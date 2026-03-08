// Copyright 2026 pugur
// This source code is licensed under the Apache License, Version 2.0 with LLVM
// Exceptions which can be found in the LICENSE file.

#pragma once

#include "base/int.h"

namespace ir {

enum class OpCode : u8 {
  Add = 0,
  Sub = 1,
  Mul = 2,
  Div = 3,
  Mod = 4,
};

}  // namespace ir
