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
target_include_directories(stb INTERFACE ${stb_SOURCE_DIR})

function(add_sd_plugin_example name)
    set(tgt example-aclp-sd-${name})
    add_executable(${tgt} ep-${name}.cpp)
    target_link_libraries(${tgt} PRIVATE
        ac::local
        ac::jalog
        ac-test-data::sd
        aclp::sd-info
        ac::sd.cpp-schema
        stb
    )
    set_target_properties(${tgt} PROPERTIES FOLDER example)
endfunction()

add_sd_plugin_example(text-to-image)


