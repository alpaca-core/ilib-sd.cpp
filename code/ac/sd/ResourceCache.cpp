// Copyright (c) Alpaca Core
// SPDX-License-Identifier: MIT
//
#include "ResourceCache.hpp"

#include <stable-diffusion.h>

namespace ac::sd {

local::ResourceLock<SDModelResource> ResourceCache::getOrCreateModel(std::string_view modelPath, Model::Params params) {
    return m_modelsManager.findOrCreateResource<SDModelResource>(ModelKey{modelPath.data(), params}, [&] {
        return std::make_shared<SDModelResource>(modelPath, params);
    });
}

}
