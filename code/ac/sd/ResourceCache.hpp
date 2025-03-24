// Copyright (c) Alpaca Core
// SPDX-License-Identifier: MIT
//
#pragma once
#include "Model.hpp"

#include <ac/local/ResourceCache.hpp>

namespace ac::sd {

class ResourceCache {
public:
    ResourceCache(local::ResourceManager& rm)
        : m_models(rm)
    {}

    struct ModelParams {
        std::string modelPath;
        Model::Params params;

        bool operator==(const ModelParams& other) const noexcept = default;
    };

    struct ModelResource : public Model, public local::Resource {
        using Model::Model;
    };

    using ModelLock = local::ResourceLock<ModelResource>;

    ModelLock getModel(ModelParams params) {
        return m_models.findOrCreate(std::move(params), [&](const ModelParams& key) {
            return std::make_shared<ModelResource>(key.modelPath, key.params);
        });
    }

private:
    local::ResourceCache<ModelParams, ModelResource> m_models;
};

}
