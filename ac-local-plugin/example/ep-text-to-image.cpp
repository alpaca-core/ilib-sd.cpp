// Copyright (c) Alpaca Core
// SPDX-License-Identifier: MIT
//
#include <ac/local/Model.hpp>
#include <ac/local/Instance.hpp>
#include <ac/local/ModelLoaderRegistry.hpp>
#include <ac/local/Lib.hpp>

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

int main() try {
    ac::jalog::Instance jl;
    jl.setup().add<ac::jalog::sinks::DefaultSink>();

    ac::local::Lib::loadPlugin(ACLP_sd_PLUGIN_FILE);;

    auto model = ac::local::Lib::modelLoaderRegistry().createModel(
        {
            .inferenceType = "stable-diffusion",
            .assets = {
                {.path = AC_TEST_DATA_SD_DIR "/sd-v1-4.ckpt"}
            }
        },
        {}, // no params
        {} // empty progress callback
    );

    auto instance = model->createInstance("general", {});

    std::string prompt = "A painting of a beautiful sunset over a calm lake.";
    std::cout << "Generate image with text: \""<< prompt <<"\": \n\n";

    auto result = instance->runOp("textToImage", {{"prompt", prompt}}, {});

    // generate the image
    std::string outputPath = "output.png";
    size_t posBeforeExt    = outputPath.find_last_of(".");
    std::string dummy_name = posBeforeExt != std::string::npos ?
        outputPath.substr(0, posBeforeExt) : outputPath;
    for (size_t i = 0; i < 1; i++)
    {
        std::string final_image_path = i > 0 ?
        dummy_name + "_" + std::to_string(i + 1) + ".png" :
        dummy_name + ".png";
        auto width = result["width"].get<int>();
        auto height = result["height"].get<int>();
        auto channel = result["channel"].get<int>();
        auto data = result["data"].get<ac::Blob>();
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
