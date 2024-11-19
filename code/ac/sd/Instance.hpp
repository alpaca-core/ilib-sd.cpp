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

enum SampleMethod {
    EULER_A,
    EULER,
    HEUN,
    DPM2,
    DPMPP2S_A,
    DPMPP2M,
    DPMPP2Mv2,
    IPNDM,
    IPNDM_V,
    LCM,

    COUNT
};

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

    std::unique_ptr<ImageResult> textToImage(
        std::string_view prompt,
        std::string_view negativePrompt,
        // int clip_skip,
        // float cfg_scale,
        // float guidance,
        int width,
        int height,
        SampleMethod sampleMethod,
        int sampleSteps
        // float control_strength,
        // float style_strength,
        // bool normalize_input,
        // const char* input_id_images_path
        );
    std::unique_ptr<ImageResult> imageToImage(std::string_view imagePath);

private:

    Model& m_model;
    InitParams m_params;
};

} // namespace ac::sd
