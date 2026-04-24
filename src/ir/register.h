// Copyright 2026 pugur
// This source code is licensed under the Apache License, Version 2.0 with LLVM
// Exceptions which can be found in the LICENSE file.

#pragma once

#include "fpag/base/numeric.h"
#include "ir/type.h"

namespace ir {

struct Register {
  Type type;
  u32 def_idx;
};

struct RegisterId {
  u32 id = 0;

  constexpr RegisterId() = default;
  // NOLINTNEXTLINE(google-explicit-constructor,runtime/explicit)
  constexpr RegisterId(u32 i) : id(i) {}
};

constexpr RegisterId kInvalidRegisterId = RegisterId(0xffffffff);

}  // namespace ir
