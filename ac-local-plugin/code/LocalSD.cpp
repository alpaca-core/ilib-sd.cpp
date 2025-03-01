// Copyright (c) Alpaca Core
// SPDX-License-Identifier: MIT
//

#include <ac/sd/Instance.hpp>
#include <ac/sd/Init.hpp>
#include <ac/sd/Model.hpp>
#include <ac/sd/ResourceCache.hpp>

#include <ac/local/Provider.hpp>
#include <ac/local/ProviderSessionContext.hpp>

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
                throw_ex{} << "dummy: unknown op: " << f.op;
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

xec::coro<void> Sd_runModel(IoEndpoint& io, std::unique_ptr<sd::Model> model) {
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

    Runner runner(*model);
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

        std::unique_ptr<sd::Model> model;
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

            model = std::make_unique<sd::Model>(resourceCache.getOrCreateModel(bin, lparams), lparams);

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
                co_await Sd_runModel(io, std::move(runner.model));
            }
        }
    }
    catch (io::stream_closed_error&) {
        co_return;
    }
}

class SdProvider final : public Provider {
public:
    virtual const Info& info() const noexcept override {
        static Info i = {
            .name = "ac stable-diffusion.cpp",
            .vendor = "Alpaca Core"
        };
        return i;
    }

    sd::ResourceCache m_cache;

    virtual void createSession(ProviderSessionContext ctx) override {
        co_spawn(ctx.executor.cpu, Sd_runSession(std::move(ctx.endpoint.session), m_cache));
    }
};

}

} // namespace ac::local

namespace ac::sd {

void init() {
    initLibrary();
}

std::vector<ac::local::ProviderPtr> getProviders() {
    std::vector<ac::local::ProviderPtr> ret;
    ret.push_back(std::make_unique<local::SdProvider>());
    return ret;
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
        .getProviders = getProviders,
    };
}

} // namespace ac::sd

