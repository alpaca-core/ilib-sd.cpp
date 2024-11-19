// Copyright (c) Alpaca Core
// SPDX-License-Identifier: MIT
//
#pragma once
#include "export.h"

#include <astl/mem_ext.hpp>

struct sd_ctx_t;

namespace ac::sd {

class AC_SD_EXPORT Model {
public:
    struct Params {
        enum Schedule {
            DEFAULT,
            DISCRETE,
            KARRAS,
            EXPONENTIAL,
            AYS,
            GITS,

            COUNT
        };
        bool gpu = true; // try to load data on gpu
        bool VaeDecodeOnly = false;
        bool vaeTiling = false;
        bool clipOnCpu = false;
        bool controlNetCpu = false;
        bool vaeOnCpu = false;
        Schedule schedule = DEFAULT;
    };

    Model(const char* pathToModel, Params params);
    ~Model();

    sd_ctx_t* context() const noexcept { return m_ctx; }
    const Params& params() const noexcept { return m_params; }

private:
    const Params m_params;
    sd_ctx_t* m_ctx;
};
} // namespace ac::sd
