# Copyright (c) Alpaca Core
# SPDX-License-Identifier: MIT
#
add_ac_local_plugin(
    NAME stable-diffusion
    SOURCES
        LocalSD.cpp
    LIBRARIES
        ac::sd
)
