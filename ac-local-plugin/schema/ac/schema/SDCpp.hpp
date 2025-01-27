// Copyright (c) Alpaca Core
// SPDX-License-Identifier: MIT
//
#pragma once
#include <ac/schema/Field.hpp>
#include <ac/Dict.hpp>
#include <vector>
#include <string>
#include <tuple>

namespace ac::schema {

struct SDCppInterface {
    static inline constexpr std::string_view id = "stable-diffusion.cpp";
    static inline constexpr std::string_view description = "ilib-sd.cpp-specific interface";

    struct OpTextToImage {
        static inline constexpr std::string_view id = "textToImage";
        static inline constexpr std::string_view description = "Run the stable-diffusion.cpp inference and produce image from text";

        struct Params {
            Field<std::string> prompt;
            Field<std::string> negativePrompt = Default("");
            Field<int16_t> width = Default(256);
            Field<int16_t> height = Default(256);
            Field<int16_t> steps = Default(20);

            template <typename Visitor>
            void visitFields(Visitor& v) {
                v(prompt, "prompt", "Image description text");
                v(negativePrompt, "negativePrompt", "Negative image description text");
                v(width, "width", "Image width");
                v(height, "height", "Image height");
                v(steps, "steps", "Number of steps");
            }
        };

        struct Return {
            Field<int16_t> width;
            Field<int16_t> height;
            Field<int16_t> channel;
            Field<ac::Blob> data;

            template <typename Visitor>
            void visitFields(Visitor& v) {
                v(width, "width", "Image width");
                v(height, "height", "Image height");
                v(channel, "channel", "Number of channels");
                v(data, "data", "Image data");
            }
        };
    };

        struct OpImageToImage {
        static inline constexpr std::string_view id = "imageToImage";
        static inline constexpr std::string_view description = "Run the stable-diffusion.cpp inference and produce image from image and text";

        struct Params {
            Field<std::string> prompt;
            Field<std::string> imagePath;
            Field<std::string> negativePrompt = Default("");
            Field<int16_t> width = Default(256);
            Field<int16_t> height = Default(256);
            Field<int16_t> steps = Default(20);

            template <typename Visitor>
            void visitFields(Visitor& v) {
                v(prompt, "prompt", "Image description text");
                v(imagePath, "imagePath", "Path to the image");
                v(negativePrompt, "negativePrompt", "Negative image description text");
                v(width, "width", "Image width");
                v(height, "height", "Image height");
                v(steps, "steps", "Number of steps");
            }
        };

        struct Return {
            Field<int16_t> width;
            Field<int16_t> height;
            Field<int16_t> channel;
            Field<ac::Blob> data;

            template <typename Visitor>
            void visitFields(Visitor& v) {
                v(width, "width", "Image width");
                v(height, "height", "Image height");
                v(channel, "channel", "Number of channels");
                v(data, "data", "Image data");
            }
        };
    };

    using Ops = std::tuple<OpTextToImage, OpImageToImage>;
};

struct SDCppProvider {
    static inline constexpr std::string_view id = "stable-diffusion.cpp";
    static inline constexpr std::string_view description = "Inference based on our fork of https://github.com/leejet/stable-diffusion.cpp";

    using Params = nullptr_t;

    struct InstanceGeneral {
        static inline constexpr std::string_view id = "general";
        static inline constexpr std::string_view description = "General instance";

        using Params = nullptr_t;

        using Interfaces = std::tuple<SDCppInterface>;
    };

    using Instances = std::tuple<InstanceGeneral>;
};

} // namespace ac::local::schema
