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
#include <ac/schema/FrameHelpers.hpp>
#include <ac/schema/StateChange.hpp>
#include <ac/schema/Error.hpp>
#include <ac/schema/OpTraits.hpp>

#include <ac/frameio/IoEndpoint.hpp>

#include <ac/xec/coro.hpp>
#include <ac/xec/co_spawn.hpp>
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

namespace {

struct LocalSD {
    Backend& m_backend;
    sd::ResourceCache& m_resourceCache;
public:
    LocalSD(Backend& backend, sd::ResourceCache& resourceCache)
        : m_backend(backend)
        , m_resourceCache(resourceCache)
    {}

    static Frame unknownOpError(const Frame& f) {
        return Frame_from(schema::Error{}, "sd: unknown op: " + f.op);
    }

    static sd::Model::Params ModelParams_fromSchema(sc::StateSD::OpLoadModel::Params& schemaParams) {
        sd::Model::Params ret;
        ret.clip_g_path = schemaParams.clip_g_path.valueOr("");
        // TODO: fill in the rest of the params
        return ret;
    }

    static sd::Instance::InitParams InstanceParams_fromSchema(sc::StateModelLoaded::OpStartInstance::Params&) {
        sd::Instance::InitParams ret;
        return ret;
    }

    sc::StateInstance::OpTextToImage::Return opTextToImage(
        sd::Instance& instance,
        const sc::StateInstance::OpTextToImage::Params& params) {

        auto res = instance.textToImage({
            .prompt = params.prompt.value(),
            .negativePrompt = params.negativePrompt.value(),
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

    sc::StateInstance::OpImageToImage::Return opImageToImage(
        sd::Instance& instance,
        const sc::StateInstance::OpImageToImage::Params& params) {

        auto res = instance.imageToImage({
            {
                .prompt = params.prompt.value(),
                .negativePrompt = params.negativePrompt.value(),
                .width = int16_t(params.width.value()),
                .height = int16_t(params.height.value()),
                .sampleSteps = int16_t(params.steps.value()),
            },
            .imagePath = params.imagePath.value()
        });

        return {
            .width = res->width,
            .height = res->height,
            .channel = res->channel,
            .data = ac::Blob(res->data, res->data + res->width * res->height * res->channel)
        };
    }

    xec::coro<void> runInstance(IoEndpoint& io, sd::Instance& instance) {
        using Schema = sc::StateInstance;
        co_await io.push(Frame_from(schema::StateChange{}, Schema::id));

        while(true) {
            auto f = co_await io.poll();
            Frame err;

            try {
                if (auto iparams = Frame_optTo(schema::OpParams<Schema::OpTextToImage>{}, *f)) {
                    co_await io.push(Frame_from(Schema::OpTextToImage{}, opTextToImage(instance, *iparams)));
                } else if (auto iparams = Frame_optTo(schema::OpParams<Schema::OpImageToImage>{}, *f)) {
                        co_await io.push(Frame_from(Schema::OpImageToImage{}, opImageToImage(instance, *iparams)));
                } else {
                    err = unknownOpError(*f);
                }
            }
            catch (std::runtime_error& e) {
                err = Frame_from(schema::Error{}, e.what());
            }

            if (!err.op.empty()) {
                co_await io.push(err);
            }
        }
    }

    xec::coro<void> runModel(IoEndpoint& io, sc::StateSD::OpLoadModel::Params& params) {
        auto modelPath = params.binPath.valueOr("");
        sd::Model::Params sdParams = ModelParams_fromSchema(params);

        auto model = m_resourceCache.getModel({
            .modelPath = modelPath,
            .params = sdParams
        });

        using Schema = sc::StateModelLoaded;
        co_await io.push(Frame_from(schema::StateChange{}, Schema::id));

        while (true) {
            auto f = co_await io.poll();
            Frame err;
            try {
                if (auto iparams = Frame_optTo(schema::OpParams<Schema::OpStartInstance>{}, *f)) {
                    sd::Instance::InitParams wiparams = InstanceParams_fromSchema(*iparams);
                    sd::Instance instance(*model, std::move(wiparams));
                    co_await runInstance(io, instance);
                }
                else {
                    err = unknownOpError(*f);
                }
            }
            catch (std::runtime_error& e) {
                 err = Frame_from(schema::Error{}, e.what());
            }

            co_await io.push(err);
        }
    }

    xec::coro<void> runSession(IoEndpoint& io) {
        using Schema = sc::StateSD;

        co_await io.push(Frame_from(schema::StateChange{}, Schema::id));

        while (true) {
            auto f = co_await io.poll();

            Frame err;

            try {
                if (auto lm = Frame_optTo(schema::OpParams<Schema::OpLoadModel>{}, * f)) {
                    co_await runModel(io, *lm);
                }
                else {
                    err = unknownOpError(*f);
                }
            }
            catch (std::runtime_error& e) {
                err = Frame_from(schema::Error{}, e.what());
            }

            co_await io.push(err);
        }
    }

    xec::coro<void> run(frameio::StreamEndpoint ep, xec::strand ex) {
        try {
            IoEndpoint io(std::move(ep), ex);
            co_await runSession(io);
        }
        catch (io::stream_closed_error&) {
            co_return;
        }
    }
};

ServiceInfo g_serviceInfo = {
    .name = "ac stable-diffusion.cpp",
    .vendor = "Alpaca Core"
};

struct SdService final : public Service {
    SdService(BackendWorkerStrand& ws) : m_workerStrand(ws) {}

    BackendWorkerStrand& m_workerStrand;
    sd::ResourceCache m_resourceCache{m_workerStrand.resourceManager};
    std::shared_ptr<LocalSD> sd;

    virtual const ServiceInfo& info() const noexcept override {
        return g_serviceInfo;
    }

    virtual void createSession(frameio::StreamEndpoint ep, Dict) override {
        sd = std::make_shared<LocalSD>(m_workerStrand.backend, m_resourceCache);
        co_spawn(m_workerStrand.executor(), sd->run(std::move(ep), m_workerStrand.executor()));
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

