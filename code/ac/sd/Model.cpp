// Copyright (c) Alpaca Core
// SPDX-License-Identifier: MIT
//
#include "Model.hpp"

#include <stable-diffusion.h>
#include <ggml.h>

#include <astl/move.hpp>
#include <stdexcept>

namespace ac::sd {

Model::Model(std::string_view pathToModel, Params params)
    : m_params(astl::move(params))
    , m_ctx(nullptr)
{
    m_ctx = new_sd_ctx(pathToModel.data(),
        params.clip_l_path.c_str(),
        params.clip_g_path.c_str(),
        params.t5xxlPath.c_str(),
        params.diffusionModelPath.c_str(),
        params.vaePath.c_str(),
        params.taesdPath.c_str(),
        params.controlnetPath.c_str(),
        params.loraModelDir.c_str(),
        params.embeddingsPath.c_str(),
        params.stackedIdEmbeddingsPath.c_str(),
        params.vaeDecodeOnly,
        params.vaeTiling,
        false, // free_params_immediately
        params.nThreads,
        getSdWeightType(params.weightType),
        getSdRngType(params.rngType),
        getSdSchedule(params.schedule),
        params.clipOnCpu,
        params.controlNetCpu,
        params.vaeOnCpu,
        params.diffusionFlashAttn);

    if (!m_ctx) {
        throw std::runtime_error("Failed to load model");
    }
}

Model::~Model() {
    free_sd_ctx(m_ctx);
}

} // namespace ac::sd
