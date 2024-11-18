// Copyright (c) Alpaca Core
// SPDX-License-Identifier: MIT
//
#pragma once
#include "export.h"

#include <astl/mem_ext.hpp>

namespace ac::sd {

class AC_SD_EXPORT Model {
public:
    struct Params {
        bool gpu = true; // try to load data on gpu
    };

    Model(const char* pathToBin, Params params);
    ~Model() = default;

    const Params& params() const noexcept { return m_params; }

private:
    const Params m_params;
};
} // namespace ac::whisper
