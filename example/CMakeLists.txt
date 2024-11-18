# Copyright (c) Alpaca Core
# SPDX-License-Identifier: MIT
#
set(TARGET example-ac-whisper-basic)

add_executable(${TARGET} e-basic.cpp)
target_link_libraries(${TARGET} PRIVATE
    ac::sd
    ac::jalog
    # ac-test-data::whisper
)
set_target_properties(${TARGET} PROPERTIES FOLDER example)

# if(TARGET ac-dev::imgui)
#     set(TARGET example-ac-whisper-gui)
#     add_executable(${TARGET} e-gui.cpp)
#     target_link_libraries(${TARGET} PRIVATE
#         ac::whisper
#         ac::jalog
#         # ac-test-data::whisper
#         ac-dev::imgui
#     )
#     set_target_properties(${TARGET} PROPERTIES FOLDER example)
# endif()
