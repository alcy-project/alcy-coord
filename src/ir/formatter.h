// Copyright 2026 pugur
// This source code is licensed under the Apache License, Version 2.0 with LLVM
// Exceptions which can be found in the LICENSE file.

#pragma once

#include <string_view>

#include "build/build_config.h"
#include "ir/opcode.h"
#include "ir/type.h"

#if !FPAG_BUILD_FLAG(USE_FMTLIB)
#include <format>
#endif

namespace ir {

constexpr std::string_view format_as(const Opcode c) {
  return std::string_view{opcode_to_str(c)};
}

constexpr std::string_view format_as(const Type t) {
  return std::string_view{type_to_str(t)};
}

}  // namespace ir

// For std::format
#if !FPAG_BUILD_FLAG(USE_FMTLIB)

template <typename T>
  requires requires(T obj) { ir::format_as(obj); }
struct std::formatter<T> : std::formatter<std::string_view> {
  auto format(T obj, format_context& ctx) const {
    return std::formatter<std::string_view>::format(ir::format_as(obj), ctx);
  }
};

#endif
