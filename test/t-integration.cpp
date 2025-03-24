// Copyright (c) Alpaca Core
// SPDX-License-Identifier: MIT
//
#include <ac/sd/Init.hpp>
#include <ac/sd/Model.hpp>
#include <ac/sd/Instance.hpp>
#include <ac/sd/ResourceCache.hpp>

#include <doctest/doctest.h>

#include "ac-test-data-sd-dir.h"

#define STB_IMAGE_WRITE_IMPLEMENTATION
#define STB_IMAGE_WRITE_STATIC
#include "stb_image_write.h"

#include <iostream>

struct GlobalFixture {
    GlobalFixture() {
        ac::sd::initLibrary();
    }
};

GlobalFixture globalFixture;
ac::local::ResourceManager rm;
ac::sd::ResourceCache resourceCache(rm);

std::string sd_v1_4_modelPath = AC_TEST_DATA_SD_DIR "/sd-v1-4.ckpt";

void saveImage(ac::sd::ImageResult* im, std::string fileName) {
    stbi_write_png(
        fileName.c_str(),
        im->width, im->height,
        im->channel, im->data,
        0, "" );
    std::cout << "Saved result image to: " << fileName << "\n";
};

bool checkImage(uint8_t* data, uint32_t width, uint32_t height, uint32_t channels, float margin) {
    float red = 0;
    float green = 0;
    float blue = 0;

    for (size_t i = 0; i < width; i++) {
        for (size_t j = 0; j < height; j++) {
            for (size_t c = 0; c < channels; c++) {
                float val = data[i*j*channels + c] / 255.f;
                switch (c) {
                    case 0: red += val;break;
                    case 1: green += val;break;
                    case 2: blue += val;break;
                    default:
                        assert("Not valid component");
                        break;
                }
            }
        }
    }

    red /= width * height;
    green /= width * height;
    blue /= width * height;

    return red > margin && red < 1.0f - margin &&
            green > margin && green < 1.0f - margin &&
            blue > margin && blue < 1.0f - margin;
}

// We'll test that the output is neither all black nor all white
// since it's hard to predict the exact output on all platforms
// due to floating point differences
TEST_CASE("inference") {
    auto model = resourceCache.getModel({.modelPath = sd_v1_4_modelPath, .params = {}});
    REQUIRE(!!model->context());

    ac::sd::Instance instance(*model, {});

    std::string inputImage = "test_text2img.png";
    // text2img
    {
        auto result = instance.textToImage({
            .prompt = "A gray background."
        });

        CHECK(checkImage(result->data, result->width, result->height, result->channel, 0.2f));
        saveImage(result.get(), inputImage);
    }

    // img2img
    {
        ac::sd::Instance::ImageToImageParams params;
        params.imagePath = inputImage;
        params.prompt = "add more blue to background color";
        auto  result = instance.imageToImage(params);
        CHECK(checkImage(result->data, result->width, result->height, result->channel, 0.2f));
    }
}
