# Copyright (c) Alpaca Core
# SPDX-License-Identifier: MIT
#
add_doctest_lib_test(integration ac-sd
    SOURCES
        t-integration.cpp
    LIBRARIES
        ac-test-data::sd
)
