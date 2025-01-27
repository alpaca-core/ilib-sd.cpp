// Copyright (c) Alpaca Core
// SPDX-License-Identifier: MIT
//

#include <ac/sd/Instance.hpp>
#include <ac/sd/Init.hpp>
#include <ac/sd/Model.hpp>

#include <ac/Dict.hpp>
#include <ac/local/Instance.hpp>
#include <ac/local/Model.hpp>
#include <ac/local/Provider.hpp>

#include <ac/schema/SDCpp.hpp>
#include <ac/local/schema/DispatchHelpers.hpp>

#include <astl/move.hpp>
#include <astl/move_capture.hpp>
#include <astl/iile.h>
#include <astl/throw_stdex.hpp>
#include <astl/workarounds.h>

#include "aclp-sd-version.h"
#include "aclp-sd-interface.hpp"

namespace ac::local {

namespace {

class SDInstance final : public Instance {
    std::shared_ptr<sd::Model> m_model;
    sd::Instance m_instance;
    schema::OpDispatcherData m_dispatcherData;
public:
    using Schema = ac::schema::SDCppProvider::InstanceGeneral;
    using Interface = ac::schema::SDCppInterface;

    SDInstance(std::shared_ptr<sd::Model> model)
        : m_model(astl::move(model))
        , m_instance(*m_model, {})
    {
        schema::registerHandlers<Interface::Ops>(m_dispatcherData, *this);
    }

    Interface::OpTextToImage::Return on(Interface::OpTextToImage, Interface::OpTextToImage::Params params) {
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

    Interface::OpImageToImage::Return on(Interface::OpImageToImage, Interface::OpImageToImage::Params params) {
        auto res = m_instance.imageToImage({
            {
            .prompt = astl::move(params.prompt.value()),
            .negativePrompt = astl::move(params.negativePrompt.value()),
            .width = int16_t(params.width.value()),
            .height = int16_t(params.height.value()),
            .sampleSteps = int16_t(params.steps.value()),
            },
            .imagePath = astl::move(params.imagePath.value())
        });

        return {
            .width = res->width,
            .height = res->height,
            .channel = res->channel,
            .data = ac::Blob(res->data, res->data + res->width * res->height * res->channel)
        };
    }

    virtual Dict runOp(std::string_view op, Dict params, ProgressCb) override {
        auto ret = m_dispatcherData.dispatch(op, astl::move(params));
        if (!ret) {
            throw_ex{} << "sd: unknown op: " << op;
        }
        return *ret;
    }
};

class SDModel final : public Model {
    std::shared_ptr<sd::Model> m_model;
public:
    using Schema = ac::schema::SDCppProvider;

    SDModel(const std::string& modelPath, sd::Model::Params params)
        : m_model(std::make_shared<sd::Model>(modelPath.c_str(), astl::move(params)))
    {}

    virtual std::unique_ptr<Instance> createInstance(std::string_view, Dict) override {
        return std::make_unique<SDInstance>(m_model);
    }
};

class SDProvider final : public Provider {
public:
    virtual const Info& info() const noexcept override {
        static Info i = {
            .name = "ac stable-diffusion.cpp",
            .vendor = "Alpaca Core"
        };
        return i;
    }

        /// Check if the model can be loaded
    virtual bool canLoadModel(const ModelAssetDesc& desc, const Dict& /*params*/) const noexcept override {
         return desc.type == "sd.cpp";
    }


    virtual ModelPtr loadModel(ModelAssetDesc desc, Dict /*params*/, ProgressCb /*progressCb*/) override {
        if (desc.assets.size() != 1) throw_ex{} << "sd: expected exactly one local asset";
        auto& bin = desc.assets.front().path;
        sd::Model::Params modelParams;
        return std::make_shared<SDModel>(bin, modelParams);
    }

    virtual frameio::SessionHandlerPtr createSessionHandler(std::string_view) override {
        return {};
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
    ret.push_back(std::make_unique<local::SDProvider>());
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

