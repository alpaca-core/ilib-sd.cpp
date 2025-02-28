# Alpaca core stable-diffusion.cpp plugin

This is a wrapper of [leejet/stable-diffusion.cpp](https://github.com/leejet/stable-diffusion.cpp).

## Build

> [!IMPORTANT]
> When cloning this repo, don't forget to fetch the submodules.
> * Either: `$ git clone https://github.com/alpaca-core/ilib-sd.cpp.git --recurse-submodules`
> * Or:
>    * `$ git clone https://github.com/alpaca-core/ilib-sd.cpp.git`
>    * `$ cd ilib-sd.cpp`
>    * `$ git submodule update --init --recursive`

## Models

Download the original `stable-diffusion` models weights. Supported formats are `.ckpt` and `.safetensors`.
Here are some examples:

- [SD v1.4](https://huggingface.co/CompVis/stable-diffusion-v-1-4-original)

```c++
    ac::sd::ResourceCache resourceCache;
    ac::sd::Model model(resourceCache.getOrCreateModel("models/sd-v1-4.ckpt", {}), {});
```

- [Flux1-dev](https://huggingface.co/second-state/FLUX.1-dev-GGUF)
```c++
    ac::sd::ResourceCache resourceCache;
    ac::sd::Model model(resourceCache.getOrCreateModel("", {}), {
        .diffusionModelPath = "models/flux1-dev-Q8_0.gguf",
        .clip_l_path = "models/text_encoders/clip_l-Q8_0.gguf",
        .t5xxlPath = "models/text_encoders/t5xxl-Q8_0.gguf",
        .vaePath = "models/vae/ae.safetensors"
    });
```

- [SD 3 medium](https://huggingface.co/stabilityai/stable-diffusion-3-medium)

```c++
    ac::sd::ResourceCache resourceCache;
    // if text encoders encoders are embedded in the model:
    ac::sd::Model model(resourceCache.getOrCreateModel("models/sd3_medium_incl_clips_t5xxlfp16.safetensors", {}), {});

    // if text endores are in separate files:
    ac::sd::Model model(resourceCache.getOrCreateModel("models/sd3_medium.safetensors", {
        .clip_l_path = "models/text_encoders/clip_l.safetensors",
        .clip_g_path = "models/text_encoders/clip_g.safetensors",
        .t5xxlPath = "models/text_encoders/t5xxl_fp16.safetensors"
    }), {});
```

- [SD 3.5 large](https://huggingface.co/stabilityai/stable-diffusion-3.5-large)

```c++
    ac::sd::ResourceCache resourceCache;
    std::string modelBinFilePath = "models/sd3.5_large.safetensors";
    ac::sd::Model model(resourceCache.getOrCreateModel("models/sd3.5_large.safetensors", {
        .clip_l_path = "models/text_encoders/clip_l.safetensors",
        .clip_g_path = "models/text_encoders/clip_g.safetensors",
        .t5xxlPath = "models/text_encoders/t5xxl_fp16.safetensors"
    }), {});
```
