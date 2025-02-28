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

inline namespace sd {

struct StateInitial {
    static constexpr auto id = "initial";
    static constexpr auto desc = "Initial state";

    struct OpLoadModel {
        static constexpr auto id = "load-model";
        static constexpr auto desc = "Load the whisper.cpp model";

        struct Params{
            Field<std::string> binPath = std::nullopt;
            Field<std::string> clip_l_path = Default("");
            Field<std::string> clip_g_path = Default("");
            Field<std::string> t5xxlPath = Default("");
            Field<std::string> diffusionModelPath = Default("");
            Field<std::string> vaePath = Default("");
            Field<std::string> taesdPath = Default("");
            Field<std::string> controlnetPath = Default("");
            Field<std::string> loraModelDir = Default("");
            Field<std::string> embeddingsPath = Default("");
            Field<std::string> stackedIdEmbeddingsPath = Default("");
            Field<bool> vaeDecodeOnly = Default(false);
            Field<bool> vaeTiling = Default(false);
            Field<bool> clipOnCpu = Default(false);
            Field<bool> controlNetCpu = Default(false);
            Field<bool> vaeOnCpu = Default(false);
            Field<bool> diffusionFlashAttn = Default(false);

            template <typename Visitor>
            void visitFields(Visitor& v) {
                v(binPath, "binPath", "Path to the file with model data.");
                v(clip_l_path, "clip_l_path", "Path to the clip_l model");
                v(clip_g_path, "clip_g_path", "Path to the clip_g model");
                v(t5xxlPath, "t5xxlPath", "Path to the t5xxl model");
                v(diffusionModelPath, "diffusionModelPath", "Path to the diffusion model");
                v(vaePath, "vaePath", "Path to the vae model");
                v(taesdPath, "taesdPath", "Path to the taesd model");
                v(controlnetPath, "controlnetPath", "Path to the controlnet model");
                v(loraModelDir, "loraModelDir", "Path to the directory of lora adapters.");
                v(embeddingsPath, "embeddingsPath", "Path to the embeddings model");
                v(stackedIdEmbeddingsPath, "stackedIdEmbeddingsPath", "Path to the stackedIdEmbeddings model");
                v(vaeDecodeOnly, "vaeDecodeOnly", "Whether to use only vae for decoding");
                v(vaeTiling, "vaeTiling", "Whether to use tiling for vae");
                v(clipOnCpu, "clipOnCpu", "Whether to use CPU for clip");
                v(controlNetCpu, "controlNetCpu", "Whether to use CPU for controlnet");
                v(vaeOnCpu, "vaeOnCpu", "Whether to use CPU for vae");
                v(diffusionFlashAttn, "diffusionFlashAttn", "Whether to use flash attention for diffusion");
            }
        };

        using Return = nullptr_t;
    };

    using Ops = std::tuple<OpLoadModel>;
    using Ins = std::tuple<>;
    using Outs = std::tuple<>;
};

struct StateModelLoaded {
    static constexpr auto id = "model-loaded";
    static constexpr auto desc = "Model loaded state";

    struct OpStartInstance {
        static constexpr auto id = "start-instance";
        static constexpr auto desc = "Start a new instance of the sd.cpp model";

        using Params = nullptr_t;
        using Return = nullptr_t;
    };

    using Ops = std::tuple<OpStartInstance>;
    using Ins = std::tuple<>;
    using Outs = std::tuple<>;
};

struct StateInstance {
    static constexpr auto id = "instance";
    static constexpr auto desc = "Instance state";

    struct OpTextToImage {
        static inline constexpr std::string_view id = "textToImage";
        static inline constexpr std::string_view desc = "Run the stable-diffusion.cpp inference and produce image from text";

        struct Params {
            Field<std::string> prompt;
            Field<std::string> negativePrompt = Default("");
            Field<int16_t> width = Default(512);
            Field<int16_t> height = Default(512);
            Field<int16_t> steps = Default(30);

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
        static inline constexpr std::string_view desc = "Run the stable-diffusion.cpp inference and produce image from image and text";

        struct Params {
            Field<std::string> prompt;
            Field<std::string> imagePath;
            Field<std::string> negativePrompt = Default("");
            Field<int16_t> width = Default(512);
            Field<int16_t> height = Default(512);
            Field<int16_t> steps = Default(30);

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
    using Ins = std::tuple<>;
    using Outs = std::tuple<>;
};

struct Interface {
    static inline constexpr std::string_view id = "stable-diffusion.cpp";
    static inline constexpr std::string_view desc = "Inference based on our fork of https://github.com/leejet/stable-diffusion.cpp";

    using States = std::tuple<StateInitial, StateModelLoaded, StateInstance>;
};

} // namespace sd

} // namespace ac::local::schema
