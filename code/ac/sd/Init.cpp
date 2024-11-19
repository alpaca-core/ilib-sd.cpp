// Copyright (c) Alpaca Core
// SPDX-License-Identifier: MIT
//
#include "Init.hpp"
#include "Logging.hpp"
#include <stable-diffusion.h>

namespace ac::sd {

namespace {
static void sdLogCb(sd_log_level_t level, const char* text, void* /*user_data*/) {
    auto len = strlen(text);

    auto jlvl = [&]() {
        switch (level) {
        case SD_LOG_ERROR: return jalog::Level::Error;
        case SD_LOG_WARN: return jalog::Level::Warning;
        case SD_LOG_INFO: return jalog::Level::Info;
        case SD_LOG_DEBUG: return jalog::Level::Debug;
        default: return jalog::Level::Critical;
        }
    }();

    // skip newlines from llama, as jalog doen't need them
    if (len > 0 && text[len - 1] == '\n') {
        --len;
    }

    log::scope.addEntry(jlvl, {text, len});
}
}


void initLibrary() {
    sd_set_log_callback(sdLogCb, nullptr);
}

} // namespace ac::sd
