// Copyright (c) Alpaca Core
// SPDX-License-Identifier: MIT
//

#include <ac/sd/Instance.hpp>
#include <ac/sd/Init.hpp>
#include <ac/sd/Model.hpp>

#include <ac/Dict.hpp>
#include <ac/local/Instance.hpp>
#include <ac/local/Model.hpp>
#include <ac/local/ModelLoader.hpp>

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
public:
    // using Schema = ac::local::schema::Whisper::InstanceGeneral;

    SDInstance(std::shared_ptr<sd::Model> model)
        : m_model(astl::move(model))
        , m_instance(*m_model, {})
    {}

    Dict textToImage(const Dict& params) {
        auto prompt = Dict_optValueAt(params, "prompt", std::string());
        auto negPrompt = Dict_optValueAt(params, "negativePrompt", std::string());
        auto width = Dict_optValueAt(params, "width", 256);
        auto height = Dict_optValueAt(params, "height", 256);
        auto steps = Dict_optValueAt(params, "steps", 20);
        auto res = m_instance.textToImage(prompt, negPrompt, width, height, ac::sd::SampleMethod::EULER_A, steps);
        Dict result;
        result["width"] = res->width;
        result["height"] = res->height;
        result["channel"] = res->channel;
        result["data"] = ac::Blob(res->data, res->data + res->width * res->height * res->channel);
        return result;
    }

    Dict ImageToImage(const Dict& params) {
        return params;
    }

    virtual Dict runOp(std::string_view op, Dict params, ProgressCb) override {
        if (op == "textToImage") {
            return textToImage(params);
        } else if (op == "imageToImage") {
            return ImageToImage(params);
        } else {
            throw_ex{} << "sd: unknown op: " << op;
            MSVC_WO_10766806();
        }
    }
};

class SDModel final : public Model {
    std::shared_ptr<sd::Model> m_model;
public:
    // using Schema = ac::local::schema::SD;

    SDModel(const std::string& modelPath, sd::Model::Params params)
        : m_model(std::make_shared<sd::Model>(modelPath.c_str(), astl::move(params)))
    {}

    virtual std::unique_ptr<Instance> createInstance(std::string_view, Dict) override {
        // switch (Schema::getInstanceById(type)) {
        // case Schema::instanceIndex<Schema::InstanceGeneral>:
        //     return std::make_unique<SDInstance>(m_model);
        // default:
        //     throw_ex{} << "sd: unknown instance type: " << type;
        //     MSVC_WO_10766806();
        // }

        return std::make_unique<SDInstance>(m_model);
    }
};

class SDModelLoader final : public ModelLoader {
public:
    virtual const Info& info() const noexcept override {
        static Info i = {
            .name = "ac stable-diffusion.cpp",
            .vendor = "Alpaca Core",
            .inferenceSchemaTypes = {"stable-diffusion"},
        };
        return i;
    }
    virtual ModelPtr loadModel(ModelDesc desc, Dict /*params*/, ProgressCb /*progressCb*/) override {
        if (desc.assets.size() != 1) throw_ex{} << "sd: expected exactly one local asset";
        auto& bin = desc.assets.front().path;
        sd::Model::Params modelParams;
        return std::make_shared<SDModel>(bin, modelParams);
    }
};
}

} // namespace ac::local

namespace ac::sd {

void init() {
    initLibrary();
}

std::vector<ac::local::ModelLoaderPtr> getLoaders() {
    std::vector<ac::local::ModelLoaderPtr> ret;
    ret.push_back(std::make_unique<local::SDModelLoader>());
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
        .getLoaders = getLoaders,
    };
}

} // namespace ac::sd

