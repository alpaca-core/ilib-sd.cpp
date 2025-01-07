# Copyright (c) Alpaca Core
# SPDX-License-Identifier: MIT
#
add_ac_local_plugin(
    NAME sd
    SOURCES
        LocalSD.cpp
    LIBRARIES
        ac::sd
        ac::sd.cpp-schema
)
