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
        "", //params.clip_l_path.c_str(),
        "", //params.clip_g_path.c_str(),
        "", //params.t5xxl_path.c_str(),
        "", //params.diffusion_model_path.c_str(),
        "", //params.vae_path.c_str(),
        "", //params.taesd_path.c_str(),
        "", //params.controlnet_path.c_str(),
        "", //params.lora_model_dir.c_str(),
        "", //params.embeddings_path.c_str(),
        "", //params.stacked_id_embeddings_path.c_str(),
        params.vaeDecodeOnly,
        params.vaeTiling,
        false, // free_params_immediately
        params.nThreads,
        getSdWeightType(params.weightType),
        getSdRngType(params.rngType),
        getSdSchedule(params.schedule),
        params.clipOnCpu,
        params.controlNetCpu,
        params.vaeOnCpu);

    if (!m_ctx) {
        throw std::runtime_error("Failed to load model");
    }
}

Model::~Model() {
    free_sd_ctx(m_ctx);
}

} // namespace ac::sd
