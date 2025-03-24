// Copyright (c) Alpaca Core
// SPDX-License-Identifier: MIT
//

#include <ac/sd/Instance.hpp>
#include <ac/sd/Init.hpp>
#include <ac/sd/Model.hpp>
#include <ac/sd/ResourceCache.hpp>

#include <ac/local/Service.hpp>
#include <ac/local/ServiceFactory.hpp>
#include <ac/local/ServiceInfo.hpp>
#include <ac/local/Backend.hpp>
#include <ac/local/BackendWorkerStrand.hpp>

#include <ac/schema/SDCpp.hpp>
#include <ac/schema/OpDispatchHelpers.hpp>

#include <ac/FrameUtil.hpp>
#include <ac/frameio/IoEndpoint.hpp>

#include <ac/xec/coro.hpp>
#include <ac/io/exception.hpp>

#include <astl/move.hpp>
#include <astl/move_capture.hpp>
#include <astl/iile.h>
#include <astl/throw_stdex.hpp>
#include <astl/workarounds.h>

#include "aclp-sd-version.h"
#include "aclp-sd-interface.hpp"

#include <iostream>

namespace ac::local {

namespace sc = schema::sd;
using namespace ac::frameio;

struct BasicRunner {
    schema::OpDispatcherData m_dispatcherData;

    Frame dispatch(Frame& f) {
        try {
            auto ret = m_dispatcherData.dispatch(f.op, std::move(f.data));
            if (!ret) {
                throw_ex{} << "sd: unknown op: " << f.op;
            }
            return {f.op, *ret};
        }
        catch (io::stream_closed_error&) {
            throw;
        }
        catch (std::exception& e) {
            return {"error", e.what()};
        }
    }
};

namespace {

xec::coro<void> Sd_runInstance(IoEndpoint& io, std::unique_ptr<sd::Instance> instance) {
    using Schema = sc::StateInstance;

    struct Runner : public BasicRunner {
        sd::Instance& m_instance;

        Runner(sd::Instance& instance)
            : m_instance(instance)
        {
            schema::registerHandlers<Schema::Ops>(m_dispatcherData, *this);
        }

        Schema::OpTextToImage::Return on(Schema::OpTextToImage, Schema::OpTextToImage::Params&& params) {
            auto res = m_instance.textToImage({
                .prompt = astl::move(params.prompt.value()),
                .negativePrompt = astl::move(params.negativePrompt.value()),
                .width = int16_t(params.width.value()),
                .height = int16_t(params.height.value()),
                .sampleSteps = int16_t(params.steps.value()),
                });

            return {
                .width = res->width,
                .height = res->height,
                .channel = res->channel,
                .data = ac::Blob(res->data, res->data + res->width * res->height * res->channel)
            };
        }

        Schema::OpImageToImage::Return on(Schema::OpImageToImage, Schema::OpImageToImage::Params&& params) {
            auto res = m_instance.imageToImage({
                {
                    .prompt = astl::move(params.prompt.value()),
                    .negativePrompt = astl::move(params.negativePrompt.value()),
                    .width = int16_t(params.width.value()),
                    .height = int16_t(params.height.value()),
                    .sampleSteps = int16_t(params.steps.value()),
                },
                astl::move(params.imagePath.value())
            });

            return {
                .width = res->width,
                .height = res->height,
                .channel = res->channel,
                .data = ac::Blob(res->data, res->data + res->width * res->height * res->channel)
            };
        }
    };

    co_await io.push(Frame_stateChange(Schema::id));

    Runner runner(*instance);
    while (true)
    {
        auto f = co_await io.poll();
        co_await io.push(runner.dispatch(*f));
    }
}

xec::coro<void> Sd_runModel(IoEndpoint& io, sd::Model& model) {
    using Schema = sc::StateModelLoaded;

    struct Runner : public BasicRunner {
        Runner(sd::Model& model)
            : lmodel(model)
        {
            schema::registerHandlers<Schema::Ops>(m_dispatcherData, *this);
        }

        sd::Model& lmodel;
        std::unique_ptr<sd::Instance> instance;

        static sd::Instance::InitParams InstanceParams_fromSchema(sc::StateModelLoaded::OpStartInstance::Params&) {
            sd::Instance::InitParams ret;
            return ret;
        }

        Schema::OpStartInstance::Return on(Schema::OpStartInstance, Schema::OpStartInstance::Params params) {
            instance = std::make_unique<sd::Instance>(lmodel, InstanceParams_fromSchema(params));
            return {};
        }
    };

    co_await io.push(Frame_stateChange(Schema::id));

    Runner runner(model);
    while (true)
    {
        auto f = co_await io.poll();
        co_await io.push(runner.dispatch(*f));
        if (runner.instance) {
            co_await Sd_runInstance(io, std::move(runner.instance));
        }
    }
}

xec::coro<void> Sd_runSession(StreamEndpoint ep, sd::ResourceCache& resourceCache) {
    using Schema = sc::StateInitial;

    struct Runner : public BasicRunner {
        Runner(sd::ResourceCache& cache)
            : resourceCache(cache)
        {
            schema::registerHandlers<Schema::Ops>(m_dispatcherData, *this);
        }

        sd::ResourceCache::ModelLock model;
        sd::ResourceCache& resourceCache;

        static sd::Model::Params ModelParams_fromSchema(sc::StateInitial::OpLoadModel::Params schemaParams) {
            sd::Model::Params ret;
            ret.clip_g_path = schemaParams.clip_g_path.valueOr("");
            // TODO: fill in the rest of the params
            return ret;
        }

        Schema::OpLoadModel::Return on(Schema::OpLoadModel, Schema::OpLoadModel::Params params) {
            auto bin = params.binPath.valueOr("");
            auto lparams = ModelParams_fromSchema(params);

            model = resourceCache.getModel({.modelPath = bin, .params = lparams});

            return {};
        }
    };

    try {
        auto ex = co_await xec::executor{};
        IoEndpoint io(std::move(ep), ex);

        co_await io.push(Frame_stateChange(Schema::id));

        Runner runner(resourceCache);

        while (true)
        {
            auto f = co_await io.poll();
            co_await io.push(runner.dispatch(*f));
            if (runner.model) {
                co_await Sd_runModel(io, *runner.model);
            }
        }
    }
    catch (io::stream_closed_error&) {
        co_return;
    }
}

ServiceInfo g_serviceInfo = {
    .name = "ac stable-diffusion.cpp",
    .vendor = "Alpaca Core"
};

struct SdService final : public Service {
    SdService(BackendWorkerStrand& ws) : m_workerStrand(ws) {}

    BackendWorkerStrand& m_workerStrand;
    sd::ResourceCache m_resourceCache{m_workerStrand.resourceManager};

    virtual const ServiceInfo& info() const noexcept override {
        return g_serviceInfo;
    }

    virtual void createSession(frameio::StreamEndpoint ep, Dict) override {
        co_spawn(m_workerStrand.executor(), Sd_runSession(std::move(ep), m_resourceCache));
    }
};

struct SdServiceFactory final : public ServiceFactory {
    virtual const ServiceInfo& info() const noexcept override {
        return g_serviceInfo;
    }
    virtual std::unique_ptr<Service> createService(Backend& backend) const override {
        auto svc = std::make_unique<SdService>(backend.gpuWorkerStrand());
        return svc;
    }
};

} // namespace

} // namespace ac::local

namespace ac::sd {

void init() {
    initLibrary();
}

std::vector<const local::ServiceFactory*> getFactories() {
    static local::SdServiceFactory factory;
    return {&factory};
}

local::PluginInterface getPluginInterface() {
    return {
        .label = "ac stable-diffusion.cpp",
        .desc = "stable-diffusion.cpp plugin for ac-local",
        .vendor = "Alpaca Core",
        .version = astl::version{
            ACLP_sd_VERSION_MAJOR, ACLP_sd_VERSION_MINOR, ACLP_sd_VERSION_PATCH
        },
        .init = init,
        .getServiceFactories = getFactories,
    };
}

} // namespace ac::sd

