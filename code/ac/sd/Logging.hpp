// Copyright (c) Alpaca Core
// SPDX-License-Identifier: MIT
//
#pragma once
#include <ac/jalog/Scope.hpp>
#include <ac/jalog/Log.hpp>

namespace ac::sd::log {
extern jalog::Scope scope;
}

#define SD_LOG(lvl, ...) AC_JALOG_SCOPE(::ac::sd::log::scope, lvl, __VA_ARGS__)
