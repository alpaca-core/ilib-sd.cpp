# Copyright (c) Alpaca Core
# SPDX-License-Identifier: MIT
#
function(add_whisper_plugin_example name)
    set(tgt example-aclp-whisper-${name})
    add_executable(${tgt} ep-${name}.cpp)
    target_link_libraries(${tgt} PRIVATE
        ac::local
        ac::jalog
        ac-test-data::whisper
        aclp::whisper-info
        ac-dev::audio
    )
    set_target_properties(${tgt} PROPERTIES FOLDER example)
endfunction()

add_whisper_plugin_example(transcribe)


