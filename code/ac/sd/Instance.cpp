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

namespace ac::sd {

Instance::Instance(Model& model, InitParams params)
    : m_model(model)
    , m_params(astl::move(params))
{}

std::unique_ptr<ImageResult> Instance::textToImage(const TextToImageParams& params) {

    uint8_t* controlImageBuffer = nullptr;
        // only if (modelParams.controlnet_path.size() > 0 && params.control_image_path.size() > 0)
    sd_image_t controlImage;
    bool willUseControlImage = m_model.params().controlnet_path.size() > 0 && params.controlImagePath.size() > 0;
    if (willUseControlImage) {
        int c = 0;
        int width = params.width;
        int height = params.height;
        controlImageBuffer = stbi_load(params.controlImagePath.c_str(), &width, &height, &c, 3);
        if (controlImageBuffer == nullptr) {
            fprintf(stderr, "load image from '%s' failed\n", params.controlImagePath.c_str());
            return nullptr;
        }
        controlImage = sd_image_t{  (uint32_t)params.width,
                                    (uint32_t)params.height,
                                    3,
                                    controlImageBuffer};
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
                    params.input_id_images_path.c_str());

    free(controlImageBuffer);

    std::unique_ptr<ImageResult> imRes = std::make_unique<ImageResult>();
    imRes->width = res->width;
    imRes->height = res->height;
    imRes->channel = res->channel;
    imRes->data = res->data;
    return imRes;
}

std::unique_ptr<ImageResult> Instance::imageToImage(std::string_view imagePath) {
    std::unique_ptr<ImageResult> res;

    (void)imagePath;

    return res;
}

} // namespace ac::sd
