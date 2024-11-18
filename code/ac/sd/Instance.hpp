// Copyright (c) Alpaca Core
// SPDX-License-Identifier: MIT
//
#pragma once
#include "export.h"

#include <astl/mem_ext.hpp>

#include <functional>
#include <string>
#include <span>

namespace ac::sd {
class Model;

class AC_SD_EXPORT Instance {
public:
    struct InitParams {
    };

    Instance(Model& model, InitParams params);
    ~Instance() = default;

    // std::string transcribe(std::span<const float> pcmf32);

private:
    // std::string runInference(std::span<const float> pcmf32);

    Model& m_model;
    InitParams m_params;
};

} // namespace ac::whisper
