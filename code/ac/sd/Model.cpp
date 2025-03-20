// Copyright (c) Alpaca Core
// SPDX-License-Identifier: MIT
//
#include "Model.hpp"

#include <stable-diffusion.h>
#include <ggml.h>

#include <astl/move.hpp>
#include <stdexcept>

namespace ac::sd {

Model::Model(std::string_view modelPath, Params params)
    : m_params(std::move(params))
    , m_ctx(new_sd_ctx(modelPath.data(),
    m_params.clip_l_path.c_str(),
    m_params.clip_g_path.c_str(),
    m_params.t5xxlPath.c_str(),
    m_params.diffusionModelPath.c_str(),
    m_params.vaePath.c_str(),
    m_params.taesdPath.c_str(),
    m_params.controlnetPath.c_str(),
    m_params.loraModelDir.c_str(),
    m_params.embeddingsPath.c_str(),
    m_params.stackedIdEmbeddingsPath.c_str(),
    m_params.vaeDecodeOnly,
    m_params.vaeTiling,
    false, // free_params_immediately
    m_params.nThreads,
    getSdWeightType(m_params.weightType),
    getSdRngType(m_params.rngType),
    getSdSchedule(m_params.schedule),
    m_params.clipOnCpu,
    m_params.controlNetCpu,
    m_params.vaeOnCpu,
    m_params.diffusionFlashAttn), free_sd_ctx)
{
    if (!m_ctx) {
        throw std::runtime_error("Failed to load model");
    }
}

} // namespace ac::sd
