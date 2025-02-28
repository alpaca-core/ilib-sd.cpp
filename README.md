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
- [SD v2.1](https://huggingface.co/stabilityai/stable-diffusion-2-1)
- [SD 3 medium](https://huggingface.co/stabilityai/stable-diffusion-3-medium)
- [SD 3.5 large](https://huggingface.co/stabilityai/stable-diffusion-3.5-large)
