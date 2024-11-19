# Copyright (c) Alpaca Core
# SPDX-License-Identifier: MIT
#

set(TARGET example-ac-sd-basic)

add_executable(${TARGET} e-basic.cpp)
target_link_libraries(${TARGET}
    PRIVATE
        ac::sd
        ac::jalog
        ac-test-data::sd
)
set_target_properties(${TARGET} PROPERTIES FOLDER example)
