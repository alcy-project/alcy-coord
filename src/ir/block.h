// Copyright 2026 pugur
// This source code is licensed under the Apache License, Version 2.0 with LLVM
// Exceptions which can be found in the LICENSE file.

#pragma once

#include "fpag/base/numeric.h"

namespace ir {

struct Block {
  u32 head;
  u32 length;

  u32 end() const { return head + length; }

  // TODO: add parameters
};

struct BlockId {
  u32 id = 0;

  constexpr BlockId() = default;
  // NOLINTNEXTLINE(google-explicit-constructor,runtime/explicit)
  constexpr BlockId(u32 i) : id(i) {}
};

}  // namespace ir
