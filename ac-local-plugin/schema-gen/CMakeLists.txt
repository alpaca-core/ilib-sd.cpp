# Copyright (c) Alpaca Core
# SPDX-License-Identifier: MIT
#
add_executable(ac-sd.cpp-schema-gen)
target_sources(ac-sd.cpp-schema-gen PRIVATE
    schema-gen.cpp
)
target_link_libraries(ac-sd.cpp-schema-gen PRIVATE
    ac::sd.cpp-schema
)
