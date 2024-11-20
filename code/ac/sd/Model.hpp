// Copyright (c) Alpaca Core
// SPDX-License-Identifier: MIT
//
#pragma once
#include "export.h"
#include "utils.hpp"

#include <astl/mem_ext.hpp>
#include <string>

struct sd_ctx_t;

namespace ac::sd {

class AC_SD_EXPORT Model {
public:
    struct Params {
        std::string clip_l_path = "";
        std::string clip_g_path = "";
        std::string t5xxlPath = "";
        std::string diffusionModelPath = "";
        std::string vaePath = "";
        std::string taesdPath = "";
        std::string controlnetPath = "";
        std::string loraModelDir = "";
        std::string embeddingsPath = "";
        std::string stackedIdEmbeddingsPath = "";
        bool vaeDecodeOnly = false;
        bool vaeTiling = false;
        bool clipOnCpu = false;
        bool controlNetCpu = false;
        bool vaeOnCpu = false;
        uint16_t nThreads = 16;
        Schedule schedule = Schedule::DEFAULT;
        RNGType rngType = RNGType::CUDA_RNG;
        WeightType weightType = WeightType::AC_TYPE_COUNT;
    };

    Model(std::string_view pathToModel, Params params);
    ~Model();

    sd_ctx_t* context() const noexcept { return m_ctx; }
    const Params& params() const noexcept { return m_params; }

private:
    const Params m_params;
    sd_ctx_t* m_ctx;
};
} // namespace ac::sd
