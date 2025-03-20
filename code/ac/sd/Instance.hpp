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
        float cfg_scale = 5.0f;
        float guidance = 3.5f;
        float eta = 0.f;
        int16_t width = 512;
        int16_t height = 512;
        SampleMethod sampleMethod = SampleMethod::EULER;
        int16_t sampleSteps = 30;
        int16_t batchCount = 1;
        int64_t seed = 42;
        std::string controlImagePath = "";
        float control_strength = 0.9f;
        float style_ratio = 20.0f;
        bool normalize_input = false;
        std::string input_id_images_path = "";
        // bool cannyPreprocess = false; // apply canny preprocessor (edge detection)
    };
    std::unique_ptr<ImageResult> textToImage(const TextToImageParams& params);

    struct ImageToImageParams : TextToImageParams {
        std::string imagePath = "";
        float strength = 0.75f;
    };
    std::unique_ptr<ImageResult> imageToImage(const ImageToImageParams& params);

private:
    sd_image_t loadImage(const std::string& path, int& width, int& height, uint8_t*& outControlImgBuffer);

    Model& m_model;
    InitParams m_params;
};

} // namespace ac::sd
