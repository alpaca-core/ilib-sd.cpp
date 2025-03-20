// Copyright (c) Alpaca Core
// SPDX-License-Identifier: MIT
//
#include "Instance.hpp"
#include "Model.hpp"
#include "Logging.hpp"

#include <stable-diffusion.h>

#include <astl/throw_stdex.hpp>
#include <astl/iile.h>
#include <astl/move.hpp>
#include <itlib/sentry.hpp>
#include <cassert>
#include <span>

#define STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_STATIC
#include <stb_image.h>

#define STB_IMAGE_RESIZE_IMPLEMENTATION
#define STB_IMAGE_RESIZE_STATIC
#include "stb_image_resize.h"

namespace ac::sd {

Instance::Instance(Model& model, InitParams params)
    : m_model(model)
    , m_params(astl::move(params))
{}

std::unique_ptr<ImageResult> Instance::textToImage(const TextToImageParams& params) {
    uint8_t* controlImageBuffer = nullptr;
    sd_image_t controlImage;
    bool willUseControlImage = m_model.params().controlnetPath.size() > 0 && params.controlImagePath.size() > 0;
    if (willUseControlImage) {
        int width = params.width;
        int height = params.height;
        controlImage = loadImage(params.controlImagePath, width, height, controlImageBuffer);
        // if (params.canny_preprocess) {  // apply preprocessor
        //     control_image->data = preprocess_canny(control_image->data,
        //                                            control_image->width,
        //                                            control_image->height,
        //                                            0.08f,
        //                                            0.08f,
        //                                            0.8f,
        //                                            1.0f,
        //                                            false);
        // }
    }

    auto res = txt2img(m_model.context(),
                    params.prompt.c_str(),
                    params.negativePrompt.c_str(),
                    params.clip_skip,
                    params.cfg_scale,
                    params.guidance,
                    params.eta,
                    params.width,
                    params.height,
                    getSdSamplerMethod(params.sampleMethod),
                    params.sampleSteps,
                    params.seed,
                    params.batchCount,
                    willUseControlImage ? &controlImage : nullptr,
                    params.control_strength,
                    params.style_ratio,
                    params.normalize_input,
                    params.input_id_images_path.c_str(),
                    nullptr, // TODO: add support for skip_layers #10
                    0,
                    0,
                    0.01,
                    0.2);

    free(controlImageBuffer);

    std::unique_ptr<ImageResult> imRes = std::make_unique<ImageResult>();
    imRes->width = res->width;
    imRes->height = res->height;
    imRes->channel = res->channel;
    imRes->data = res->data;
    return imRes;
}

std::unique_ptr<ImageResult> Instance::imageToImage(const ImageToImageParams& params) {

    uint8_t* inputImageBuffer = nullptr;
    sd_image_t inputImage;
    {
        // Load input image
        int inputWidth = 0;
        int inputHeight = 0;
        inputImage = loadImage(params.imagePath, inputWidth, inputHeight, inputImageBuffer);
        if (inputWidth <= 0 || inputHeight <= 0) {
            free(inputImageBuffer);
            throw_ex{} << "error: the width/height of image must be greater than 0\n";
        }

        if (params.width != inputWidth || params.height != inputHeight) {
            uint8_t* resizedImageBuffer = (uint8_t*)malloc(params.width * params.height * 3);
            stbir_resize(inputImageBuffer, inputWidth, inputHeight, 0,
                    resizedImageBuffer, params.width, params.height, 0, STBIR_TYPE_UINT8,
                    3 /*RGB channel*/, STBIR_ALPHA_CHANNEL_NONE, 0,
                    STBIR_EDGE_CLAMP, STBIR_EDGE_CLAMP,
                    STBIR_FILTER_BOX, STBIR_FILTER_BOX,
                    STBIR_COLORSPACE_SRGB, nullptr);
            free(inputImageBuffer);
            inputImageBuffer = resizedImageBuffer;
            inputImage.data = inputImageBuffer;
        }
    }

    uint8_t* controlImageBuffer = nullptr;
    sd_image_t controlImage;
    bool willUseControlImage = m_model.params().controlnetPath.size() > 0 && params.controlImagePath.size() > 0;
    if (willUseControlImage) {
        // Load control image
        int width = 0;
        int height = 0;
        controlImage = loadImage(params.controlImagePath, width, height, controlImageBuffer);
        // if (params.canny_preprocess) {  // apply preprocessor
        //     control_image->data = preprocess_canny(control_image->data,
        //                                            control_image->width,
        //                                            control_image->height,
        //                                            0.08f,
        //                                            0.08f,
        //                                            0.8f,
        //                                            1.0f,
        //                                            false);
        // }
    }

    // TODO: Support mask image passed from params #11
    std::vector<uint8_t> arr(params.width * params.height, 255);
    uint8_t* mask_image_buffer = arr.data();
    sd_image_t mask_image = {(uint32_t)params.width,
                            (uint32_t)params.height,
                            1,
                            mask_image_buffer};

    auto res = img2img(m_model.context(),
                        inputImage,
                        mask_image,
                        params.prompt.c_str(),
                        params.negativePrompt.c_str(),
                        params.clip_skip,
                        params.cfg_scale,
                        params.guidance,
                        params.eta,
                        params.width,
                        params.height,
                        getSdSamplerMethod(params.sampleMethod),
                        params.sampleSteps,
                        params.strength,
                        params.seed,
                        params.batchCount,
                        willUseControlImage ? &controlImage : nullptr,
                        params.control_strength,
                        params.style_ratio,
                        params.normalize_input,
                        params.input_id_images_path.c_str(),
                        nullptr, // TODO: add support for skip_layers #10
                        0,
                        0,
                        0.01,
                        0.2);

    free(controlImageBuffer);
    std::unique_ptr<ImageResult> imRes = std::make_unique<ImageResult>();
    imRes->width = res->width;
    imRes->height = res->height;
    imRes->channel = res->channel;
    imRes->data = res->data;
    return imRes;
}

sd_image_t Instance::loadImage(const std::string& path, int& width, int& height, uint8_t*& outControlImgBuffer) {
    int c = 0;
    outControlImgBuffer = stbi_load(path.c_str(), &width, &height, &c, 3);
    if (outControlImgBuffer == nullptr) {
        throw_ex{} << "load image from \""<< path << "\" failed\n";
    }
    if (c < 3) {
        free(outControlImgBuffer);
        throw_ex{} << "error: the number of channels of image must be greater than 0\n";
    }
    return sd_image_t{ (uint32_t)width, (uint32_t)height,  3, outControlImgBuffer};
}

} // namespace ac::sd
