// Copyright (c) Alpaca Core
// SPDX-License-Identifier: MIT
//
#include <ac/local/Lib.hpp>
#include <ac/local/DefaultBackend.hpp>
#include <ac/schema/BlockingIoHelper.hpp>
#include <ac/schema/FrameHelpers.hpp>

#include <ac/schema/SDCpp.hpp>

#include <ac/jalog/Instance.hpp>
#include <ac/jalog/sinks/DefaultSink.hpp>

#include "ac-test-data-sd-dir.h"
#include "aclp-sd-info.h"

#include <iostream>
#include <string>
#include <vector>

#define STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_STATIC
#include "stb_image.h"

#define STB_IMAGE_WRITE_IMPLEMENTATION
#define STB_IMAGE_WRITE_STATIC
#include "stb_image_write.h"

namespace schema = ac::schema::sd;

int main() try {
    ac::jalog::Instance jl;
    jl.setup().add<ac::jalog::sinks::DefaultSink>();

    ac::local::Lib::loadPlugin(ACLP_sd_PLUGIN_FILE);;

    ac::local::DefaultBackend backend;
    ac::schema::BlockingIoHelper sd(backend.connect("stable-diffusion.cpp", {}));

    sd.expectState<schema::StateInitial>();
    sd.call<schema::StateInitial::OpLoadModel>({
        .binPath = AC_TEST_DATA_SD_DIR "/sd-v1-4.ckpt"
    });
    sd.expectState<schema::StateModelLoaded>();

    sd.call<schema::StateModelLoaded::OpStartInstance>({});
    sd.expectState<schema::StateInstance>();

    struct ImageResult {
        int width;
        int height;
        int channel;
        std::vector<uint8_t> data;
    };

    std::vector<ImageResult> results;
    std::string prompt = "A painting of a beautiful sunset over a calm lake.";

    auto result = sd.call<schema::StateInstance::OpTextToImage>({
        .prompt = prompt
    });

    results.push_back({.width = result.width.value(),
                       .height = result.height.value(),
                       .channel = result.channel.value(),
                       .data = std::move(result.data.value())});

    std::string imagePrompt = "Make the sunset more blue.";
    std::string inputImage = AC_TEST_DATA_SD_DIR "/sunset.png";

    auto imResult = sd.call<schema::StateInstance::OpImageToImage>({
        .prompt = prompt,
        .imagePath = inputImage
    });

    results.push_back({.width = imResult.width.value(),
                        .height = imResult.height.value(),
                        .channel = imResult.channel.value(),
                        .data = std::move(imResult.data.value())});

    // generate the image
    for (size_t i = 0; i < results.size(); i++)
    {
        std::string final_image_path = "output_" + std::to_string(i + 1) + ".png";
        auto width = results[i].width;
        auto height = results[i].height;
        auto channel = results[i].channel;
        auto data = results[i].data;
        stbi_write_png(
            final_image_path.c_str(),
            width, height,
            channel, data.data(),
            0);

        std::cout << "Result is saved at: "<< final_image_path << '\n';
    }

    return 0;
}
catch (std::exception& e) {
    std::cerr << "exception: " << e.what() << "\n";
    return 1;
}
