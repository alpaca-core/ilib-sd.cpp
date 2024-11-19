// Copyright (c) Alpaca Core
// SPDX-License-Identifier: MIT
//

// trivial example of using alpaca-core's stabke-diffusion inference

// stable-diffusion
#include <ac/sd/Init.hpp>
#include <ac/sd/Model.hpp>
#include <ac/sd/Instance.hpp>

// // logging
#include <ac/jalog/Instance.hpp>
#include <ac/jalog/sinks/ColorSink.hpp>

// // model source directory
#include "ac-test-data-sd-dir.h"

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
    jl.setup().add<ac::jalog::sinks::ColorSink>();

    std::cout << "Basic example\n";

    // initialize the library
    ac::sd::initLibrary();

    // load model
    std::string modelBinFilePath = AC_TEST_DATA_SD_DIR "/sd-v1-4.ckpt";
    ac::sd::Model model(modelBinFilePath.c_str(), {});

    // create inference instance
    ac::sd::Instance instance(model, {});

    std::string prompt = "A painting of a beautiful sunset over a calm lake.";
    std::cout << "Generate image with text: \""<< prompt <<"\": \n\n";

    // generate the image
    auto res = instance.textToImage(prompt, "", 256, 256, ac::sd::SampleMethod::EULER_A, 20);
    auto results = res.get();

    std::string outputPath = "output.png";
    size_t posBeforeExt    = outputPath.find_last_of(".");
    std::string dummy_name = posBeforeExt != std::string::npos ?
        outputPath.substr(0, posBeforeExt) : outputPath;
    for (size_t i = 0; i < 1; i++)
    {
        std::string final_image_path = i > 0 ?
        dummy_name + "_" + std::to_string(i + 1) + ".png" :
        dummy_name + ".png";
        stbi_write_png(
            final_image_path.c_str(),
            results[i].width, results[i].height,
            results[i].channel, results[i].data,
            0, "" );
    }


    std::cout << res << std::endl;

    return 0;
}
catch (const std::exception& e) {
    std::cerr << "Error: " << e.what() << std::endl;
    return 1;
}
