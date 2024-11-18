# Copyright (c) Alpaca Core
# SPDX-License-Identifier: MIT
#
icm_add_lib(ac-sd AC_SD)
add_library(ac::sd ALIAS ac-sd)
target_include_directories(ac-sd INTERFACE .)
target_link_libraries(ac-sd
    PRIVATE
        ac::jalog
    PUBLIC
        splat::splat
        ac::astl
        itlib::itlib
)
target_sources(ac-sd PRIVATE
    ac/sd/export.h
    ac/sd/Init.hpp
    ac/sd/Init.cpp
    ac/sd/Logging.hpp
    ac/sd/Logging.cpp
    ac/sd/Model.hpp
    ac/sd/Model.cpp
    ac/sd/Instance.hpp
    ac/sd/Instance.cpp
)
