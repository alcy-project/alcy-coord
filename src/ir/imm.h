// Copyright 2026 pugur
// This source code is licensed under the Apache License, Version 2.0 with LLVM
// Exceptions which can be found in the LICENSE file.

#pragma once

#include "fpag/base/numeric.h"

namespace ir {

struct ImmIId {
  u32 id = 0;

  ImmIId() = default;
  // NOLINTNEXTLINE(google-explicit-constructor,runtime/explicit)
  ImmIId(u32 i) : id(i) {}
};

struct ImmFId {
  u32 id = 0;

  constexpr ImmFId() = default;
  // NOLINTNEXTLINE(google-explicit-constructor,runtime/explicit)
  constexpr ImmFId(u32 i) : id(i) {}
};

}  // namespace ir
