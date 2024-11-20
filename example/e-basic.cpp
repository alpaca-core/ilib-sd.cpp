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

    std::vector<std::unique_ptr<ac::sd::ImageResult>> results;

    // generate the image
    results.push_back(instance.textToImage({
        .prompt = prompt
    }));

    std::string inputImage = "/Users/pacominev/Downloads/918-Spyder-mittig.jpg";
    std::string inputPrompt = "Make the car blue.";

    ac::sd::Instance::ImageToImageParams params;
    params.imagePath = inputImage;
    params.prompt = inputPrompt;
    results.push_back(instance.imageToImage(params));

    for (size_t i = 0; i < results.size(); i++) {
        if (results[i] == nullptr) {
            continue;
        }

        std::string final_image_path = "output_" + std::to_string(i + 1) + ".png";
        stbi_write_png(
            final_image_path.c_str(),
            results[i]->width, results[i]->height,
            results[i]->channel, results[i]->data,
            0, "" );
        std::cout << "Saved result image to: " << final_image_path << "\n";
    }

    return 0;
}
catch (const std::exception& e) {
    std::cerr << "Error: " << e.what() << std::endl;
    return 1;
}
