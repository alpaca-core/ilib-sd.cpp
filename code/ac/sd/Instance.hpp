// Copyright (c) Alpaca Core
// SPDX-License-Identifier: MIT
//
#pragma once
#include "export.h"
#include "utils.hpp"

#include <astl/mem_ext.hpp>

#include <functional>
#include <string>
#include <span>

namespace ac::sd {
class Model;

struct ImageResult {
    uint32_t width = 0;
    uint32_t height = 0;
    uint32_t channel = 0;
    uint8_t* data = nullptr;

    ~ImageResult() {
        if (data) {
            free(data);
        }
    }
};

class AC_SD_EXPORT Instance {
public:
    struct InitParams {
    };

    Instance(Model& model, InitParams params);
    ~Instance() = default;

    struct TextToImageParams {
        std::string prompt = "";
        std::string negativePrompt = "";
        int16_t clip_skip = -1;
        float cfg_scale = 7.0f;
        float guidance = 3.5f;
        int16_t width = 512;
        int16_t height = 512;
        SampleMethod sampleMethod = SampleMethod::EULER_A;
        int16_t sampleSteps = 20;
        int16_t batchCount = 1;
        int64_t seed = 42;
        std::string controlImagePath = "";
        float control_strength = 0.9f;
        float style_ratio = 20.0f;
        bool normalize_input = false;
        std::string input_id_images_path = "";
    };
    std::unique_ptr<ImageResult> textToImage(const TextToImageParams& params);

    std::unique_ptr<ImageResult> imageToImage(std::string_view imagePath);

private:

    Model& m_model;
    InitParams m_params;
};

} // namespace ac::sd
