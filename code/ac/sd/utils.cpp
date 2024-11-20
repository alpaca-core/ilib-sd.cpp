// Copyright (c) Alpaca Core
// SPDX-License-Identifier: MIT
//
#include "utils.hpp"

namespace ac::sd {
schedule_t getSdSchedule(Schedule schedule) {
    switch (schedule) {
    case Schedule::DEFAULT:
        return schedule_t::DEFAULT;
    case Schedule::DISCRETE:
        return schedule_t::DISCRETE;
    case Schedule::KARRAS:
        return schedule_t::KARRAS;
    case Schedule::EXPONENTIAL:
        return schedule_t::EXPONENTIAL;
    case Schedule::AYS:
        return schedule_t::AYS;
    case Schedule::GITS:
        return schedule_t::GITS;
    default:
        return schedule_t::DEFAULT;
    }
}

rng_type_t getSdRngType(RNGType rngType) {
    switch (rngType) {
    case RNGType::STD_DEFAULT_RNG:
        return rng_type_t::STD_DEFAULT_RNG;
    case RNGType::CUDA_RNG:
        return rng_type_t::CUDA_RNG;
    default:
        return rng_type_t::CUDA_RNG;
    }
}

sd_type_t getSdWeightType(WeightType type) {
    switch (type) {
        case WeightType::AC_TYPE_F32:
            return SD_TYPE_F32;
        case WeightType::AC_TYPE_F16:
            return SD_TYPE_F16;
        case WeightType::AC_TYPE_Q4_0:
            return SD_TYPE_Q4_0;
        case WeightType::AC_TYPE_Q4_1:
            return SD_TYPE_Q4_1;
        case WeightType::AC_TYPE_Q5_0:
            return SD_TYPE_Q5_0;
        case WeightType::AC_TYPE_Q5_1:
            return SD_TYPE_Q5_1;
        case WeightType::AC_TYPE_Q8_0:
            return SD_TYPE_Q8_0;
        case WeightType::AC_TYPE_Q8_1:
            return SD_TYPE_Q8_1;
        case WeightType::AC_TYPE_Q2_K:
            return SD_TYPE_Q2_K;
        case WeightType::AC_TYPE_Q3_K:
            return SD_TYPE_Q3_K;
        case WeightType::AC_TYPE_Q4_K:
            return SD_TYPE_Q4_K;
        case WeightType::AC_TYPE_Q5_K:
            return SD_TYPE_Q5_K;
        case WeightType::AC_TYPE_Q6_K:
            return SD_TYPE_Q6_K;
        case WeightType::AC_TYPE_Q8_K:
            return SD_TYPE_Q8_K;
        case WeightType::AC_TYPE_IQ2_XXS:
            return SD_TYPE_IQ2_XXS;
        case WeightType::AC_TYPE_IQ2_XS:
            return SD_TYPE_IQ2_XS;
        case WeightType::AC_TYPE_IQ3_XXS:
            return SD_TYPE_IQ3_XXS;
        case WeightType::AC_TYPE_IQ1_S:
            return SD_TYPE_IQ1_S;
        case WeightType::AC_TYPE_IQ4_NL:
            return SD_TYPE_IQ4_NL;
        case WeightType::AC_TYPE_IQ3_S:
            return SD_TYPE_IQ3_S;
        case WeightType::AC_TYPE_IQ2_S:
            return SD_TYPE_IQ2_S;
        case WeightType::AC_TYPE_IQ4_XS:
            return SD_TYPE_IQ4_XS;
        case WeightType::AC_TYPE_I8:
            return SD_TYPE_I8;
        case WeightType::AC_TYPE_I16:
            return SD_TYPE_I16;
        case WeightType::AC_TYPE_I32:
            return SD_TYPE_I32;
        case WeightType::AC_TYPE_I64:
            return SD_TYPE_I64;
        case WeightType::AC_TYPE_F64:
            return SD_TYPE_F64;
        case WeightType::AC_TYPE_IQ1_M:
            return SD_TYPE_IQ1_M;
        case WeightType::AC_TYPE_BF16:
            return SD_TYPE_BF16;
        case WeightType::AC_TYPE_Q4_0_4_4:
            return SD_TYPE_Q4_0_4_4;
        case WeightType::AC_TYPE_Q4_0_4_8:
            return SD_TYPE_Q4_0_4_8;
        case WeightType::AC_TYPE_Q4_0_8_8:
            return SD_TYPE_Q4_0_8_8;
        default:
            return SD_TYPE_COUNT;
    }
}

sample_method_t getSdSamplerMethod(SampleMethod sampler) {
    switch (sampler)
    {
    case SampleMethod::EULER_A:
        return sample_method_t::EULER_A;
    case SampleMethod::EULER:
        return sample_method_t::EULER;
    case SampleMethod::HEUN:
        return sample_method_t::HEUN;
    case SampleMethod::DPM2:
        return sample_method_t::DPM2;
    case SampleMethod::DPMPP2S_A:
        return sample_method_t::DPMPP2S_A;
    case SampleMethod::DPMPP2M:
        return sample_method_t::DPMPP2M;
    case SampleMethod::DPMPP2Mv2:
        return sample_method_t::DPMPP2Mv2;
    case SampleMethod::IPNDM:
        return sample_method_t::IPNDM;
    case SampleMethod::IPNDM_V:
        return sample_method_t::IPNDM_V;
    case SampleMethod::LCM:
        return sample_method_t::LCM;
    default:
        return sample_method_t::EULER_A;
    }
}


} // namespace ac::sd
