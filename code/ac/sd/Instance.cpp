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

namespace ac::sd {

Instance::Instance(Model& model, InitParams params)
    : m_model(model)
    , m_params(astl::move(params))
{}

std::unique_ptr<ImageResult> Instance::textToImage(
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
    ) {

    auto res = txt2img(m_model.context(),
                    prompt.data(),
                    negativePrompt.data(),
                    -1, //params.clip_skip,
                    7.0f, //params.cfg_scale,
                    3.5f, //params.guidance,
                    width,
                    height,
                    sample_method_t(sampleMethod),
                    sampleSteps,
                    42, //params.seed,
                    1, //params.batch_count,
                    nullptr,// control_image, // only if control_image_path is passed as an argument
                    0.9f, //params.control_strength,
                    20.f, //params.style_ratio,
                    false, //params.normalize_input,
                    "");//params.input_id_images_path.c_str());

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
