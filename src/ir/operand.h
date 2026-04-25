// Copyright 2026 pugur
// This source code is licensed under the Apache License, Version 2.0 with LLVM
// Exceptions which can be found in the LICENSE file.

#pragma once

#include "ir/common.h"
#include "ir/type.h"

namespace ir {

enum class OperandTag : u8 {
  Register,
  Block,
  Immutable,
  Unknown,
};

struct Operand {
  OperandTag tag;
  Type type;

  union Data {
    RegisterId register_id;
    BlockId block_id;
    ImmutableId immutable_id;
  } data;
};

}  // namespace ir
