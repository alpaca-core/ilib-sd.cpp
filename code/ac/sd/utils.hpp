// Copyright (c) Alpaca Core
// SPDX-License-Identifier: MIT
//
#pragma once
#include <stable-diffusion.h>

namespace ac::sd {

enum Schedule {
    DEFAULT,
    DISCRETE,
    KARRAS,
    EXPONENTIAL,
    AYS,
    GITS,

    SCHEDULE_COUNT
};

enum RNGType {
    STD_DEFAULT_RNG,
    CUDA_RNG,

    RNG_COUNT
};

// same as enum ggml_type
enum WeightType {
    AC_TYPE_F32  = 0,
    AC_TYPE_F16  = 1,
    AC_TYPE_Q4_0 = 2,
    AC_TYPE_Q4_1 = 3,
    // AC_TYPE_Q4_2 = 4, support has been removed
    // AC_TYPE_Q4_3 = 5, support has been removed
    AC_TYPE_Q5_0     = 6,
    AC_TYPE_Q5_1     = 7,
    AC_TYPE_Q8_0     = 8,
    AC_TYPE_Q8_1     = 9,
    AC_TYPE_Q2_K     = 10,
    AC_TYPE_Q3_K     = 11,
    AC_TYPE_Q4_K     = 12,
    AC_TYPE_Q5_K     = 13,
    AC_TYPE_Q6_K     = 14,
    AC_TYPE_Q8_K     = 15,
    AC_TYPE_IQ2_XXS  = 16,
    AC_TYPE_IQ2_XS   = 17,
    AC_TYPE_IQ3_XXS  = 18,
    AC_TYPE_IQ1_S    = 19,
    AC_TYPE_IQ4_NL   = 20,
    AC_TYPE_IQ3_S    = 21,
    AC_TYPE_IQ2_S    = 22,
    AC_TYPE_IQ4_XS   = 23,
    AC_TYPE_I8       = 24,
    AC_TYPE_I16      = 25,
    AC_TYPE_I32      = 26,
    AC_TYPE_I64      = 27,
    AC_TYPE_F64      = 28,
    AC_TYPE_IQ1_M    = 29,
    AC_TYPE_BF16     = 30,
    // AC_TYPE_Q4_0_4_4 = 31, support has been removed from gguf files
    // AC_TYPE_Q4_0_4_8 = 32,
    // AC_TYPE_Q4_0_8_8 = 33,
    AC_TYPE_TQ1_0   = 34,
    AC_TYPE_TQ2_0   = 35,
    // AC_TYPE_IQ4_NL_4_4 = 36,
    // AC_TYPE_IQ4_NL_4_8 = 37,
    // AC_TYPE_IQ4_NL_8_8 = 38,
    AC_TYPE_COUNT,
};

schedule_t getSdSchedule(Schedule schedule);
rng_type_t getSdRngType(RNGType rngType);
sd_type_t getSdWeightType(WeightType type);

enum SampleMethod {
    EULER_A,
    EULER,
    HEUN,
    DPM2,
    DPMPP2S_A,
    DPMPP2M,
    DPMPP2Mv2,
    IPNDM,
    IPNDM_V,
    LCM,
    DDIM_TRAILING,
    TCD,

    COUNT
};

sample_method_t getSdSamplerMethod(SampleMethod sampler);

} // namespace ac::sd
