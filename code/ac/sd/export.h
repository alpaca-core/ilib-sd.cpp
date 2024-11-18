// Copyright (c) Alpaca Core
// SPDX-License-Identifier: MIT
//
#pragma once
#include <splat/symbol_export.h>

#if AC_SD_SHARED
#   if BUILDING_AC_SD
#       define AC_SD_EXPORT SYMBOL_EXPORT
#   else
#       define AC_SD_EXPORT SYMBOL_IMPORT
#   endif
#else
#   define AC_SD_EXPORT
#endif
