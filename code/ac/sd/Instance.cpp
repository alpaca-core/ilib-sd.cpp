// Copyright (c) Alpaca Core
// SPDX-License-Identifier: MIT
//
#include "Instance.hpp"
#include "Model.hpp"
#include "Logging.hpp"

// #include <whisper.h>

#include <astl/throw_stdex.hpp>
#include <astl/iile.h>
#include <astl/move.hpp>
#include <itlib/sentry.hpp>
#include <cassert>
#include <span>

namespace ac::sd {

Instance::Instance(Model& model, InitParams params)
    : m_model(model)
    , m_params(astl::move(params))
{}

// std::string Instance::transcribe(std::span<const float> pcmf32) {
//     return runInference(pcmf32);
// }

// std::string Instance::runInference(std::span<const float> pcmf32) {
//     auto wparams = whisperFromInstanceParams(m_params);

//     if (whisper_full_with_state(m_model.context(), m_state.get(), wparams, pcmf32.data(), int(pcmf32.size())) != 0) {
//         throw_ex{} << "Failed to process audio!";
//     }

//     std::string result;
//     const int n_segments = whisper_full_n_segments_from_state(m_state.get());
//     for (int i = 0; i < n_segments; ++i) {
//         const char * text = whisper_full_get_segment_text_from_state(m_state.get(), i);
//         result += std::string(text) + "\n";
//     }

//     return result;
// }
} // namespace ac::whisper
