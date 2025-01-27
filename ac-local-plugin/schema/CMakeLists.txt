# Copyright (c) Alpaca Core
# SPDX-License-Identifier: MIT
#
add_library(ac-sd.cpp-schema INTERFACE)
add_library(ac::sd.cpp-schema ALIAS ac-sd.cpp-schema)
target_link_libraries(ac-sd.cpp-schema INTERFACE ac::frame)
target_include_directories(ac-sd.cpp-schema INTERFACE .)
