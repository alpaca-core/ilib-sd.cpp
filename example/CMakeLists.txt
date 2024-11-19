# Copyright (c) Alpaca Core
# SPDX-License-Identifier: MIT
#

CPMAddPackage(
    NAME stb
    VERSION 1.0
    GIT_REPOSITORY https://github.com/nothings/stb.git
    GIT_TAG 5c205738c191bcb0abc65c4febfa9bd25ff35234
)
add_library(stb INTERFACE)
target_include_directories(stb INTERFACE ${dr_libs_SOURCE_DIR})

set(TARGET example-ac-sd-basic)

add_executable(${TARGET} e-basic.cpp)
target_link_libraries(${TARGET}
    PRIVATE
        ac::sd
        ac::jalog
        ac-test-data::sd
        stb
)
set_target_properties(${TARGET} PROPERTIES FOLDER example)
