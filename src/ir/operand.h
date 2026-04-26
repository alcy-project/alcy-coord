// Copyright 2026 pugur
// This source code is licensed under the Apache License, Version 2.0 with LLVM
// Exceptions which can be found in the LICENSE file.

#pragma once

#include "base/id.h"
#include "ir/common.h"
#include "ir/type.h"

namespace ir {

enum class OperandTag : u8 {
  Register,
  Function,
  Block,
  Immutable,
  ExternalFunction,
  Unknown,
};

struct Operand {
  OperandTag tag;
  Type type;

  union {
    RegisterId register_id;
    FunctionId function_id;
    BlockId block_id;
    ImmutableId immutable_id;
    ExternalFunctionId external_function_id;
  } data;
};

constexpr Operand kInvalidOperand = {
    .tag = OperandTag::Unknown,
    .type = Type::Void,
    .data = {.register_id = RegisterId(base::kInvalidId)},
};

}  // namespace ir
