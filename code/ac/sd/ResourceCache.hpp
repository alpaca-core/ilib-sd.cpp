// Copyright (c) Alpaca Core
// SPDX-License-Identifier: MIT
//
#pragma once
#include "Model.hpp"

#include <ac/local/ResourceManager.hpp>

namespace ac::sd {

struct ModelKey {
    std::string modelPath;
    Model::Params params;

    bool operator==(const ModelKey& other) const noexcept = default;
};


class AC_SD_EXPORT ResourceCache {
public:
    struct ModelParams {
        std::string modelPath;
        Model::Params params;

        bool operator==(const ModelParams& other) const noexcept = default;
    };

    struct ModelResource : public Model, local::Resource {
        ModelResource(const ModelParams& params)
            : Model(params.modelPath, params.params)
        {}
    };

    using ModelLock = local::ResourceLock<ModelResource>;

    ModelLock getModel(ModelParams params) {
        return m_modelManager.findOrCreate(std::move(params), [&](const ModelParams& key) {
            return std::make_shared<ModelResource>(key);
        });
    }

private:
    local::ResourceManager<ModelParams, ModelResource> m_modelManager;
};

}
