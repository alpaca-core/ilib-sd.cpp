# Copyright (c) Alpaca Core
# SPDX-License-Identifier: MIT
#
cmake_minimum_required(VERSION 3.24 FATAL_ERROR)

project(ilib-sd.cpp
    VERSION 1.0.0
    DESCRIPTION "Stable-diffusion.cpp plugin for the Alpaca Core Local SDK"
    LANGUAGES CXX
)

include(./get-ac-build.cmake)

#################
# cmake lib
CPMAddPackage(gh:iboB/icm@1.5.4)
list(APPEND CMAKE_MODULE_PATH
    "${icm_SOURCE_DIR}"
)

include(icm_add_lib)

include(init_ac_prj)

include(ac_build_prj_util)
include(ac_build_plugin_util)

#################
# options

option(AC_SD_BUILD_TESTS "${PROJECT_NAME}: build tests" ${testsDefault})
option(AC_SD_BUILD_EXAMPLES "${PROJECT_NAME}: build examples" ${examplesDefault})
mark_as_advanced(AC_SD_BUILD_TESTS AC_SD_BUILD_EXAMPLES)

init_ac_plugin_option(SD)

#######################################
# packages

add_ac_local(0.1.1)
CPMAddPackage(gh:iboB/splat@1.3.3)
CPMAddPackage(gh:iboB/itlib@1.11.4)

#######################################
# submodule and config

find_package(CUDAToolkit)
if(CUDAToolkit_FOUND)
    set(haveCuda YES)
    enable_language(CUDA)
    set(CMAKE_CUDA_ARCHITECTURES 70)
    message(STATUS "${CMAKE_PROJECT_NAME}: system CUDA found")
    # we should add -forward-unknown-to-host-compiler but it somehow works without it
    # could it be that it depends on the CMake version?
endif()

if(haveCuda)
    set(SD_CUBLAS ON)
endif()

if(UNIX AND APPLE)
    set(SD_METAL ON)
endif()

set(GGML_CCACHE OFF)

add_subdirectory(stable-diffusion.cpp)

#######################################
# subdirs
add_subdirectory(code)

if(AC_SD_BUILD_TESTS OR AC_SD_BUILD_EXAMPLES)
    CPMAddPackage(
        NAME ac-test-data-stable-diffusion
        VERSION 1.0.0
        GIT_REPOSITORY https://huggingface.co/alpaca-core/ac-test-data-stable-diffusion
        GIT_TAG 1c6409643e3756c98b8d117d9de134ffe6ef0218
    )
endif()

if(AC_SD_BUILD_TESTS)
    CPMAddPackage(gh:iboB/doctest-util@0.1.2)
    set_target_properties(doctest PROPERTIES FOLDER test)
    set_target_properties(doctest-main PROPERTIES FOLDER test)
    enable_testing()
    add_subdirectory(test)
endif()

if(AC_SD_BUILD_EXAMPLES)
    add_subdirectory(example)
endif()

if(BUILD_AC_SD_PLUGIN)
    add_subdirectory(ac-local-plugin)
endif()
