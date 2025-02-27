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
    local::ResourceLock<SDModelResource> getOrCreateModel(std::string_view gguf, Model::Params params);

private:
    local::ResourceManager<ModelKey> m_modelsManager;
};

}
