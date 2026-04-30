// Copyright 2026 pugur
// This source code is licensed under the Apache License, Version 2.0 with LLVM
// Exceptions which can be found in the LICENSE file.

#pragma once

#include "base/id.h"
#include "fpag/base/numeric.h"
#include "ir/common.h"
#include "ir/type.h"

namespace ir {

struct Block {
  InstructionId head_id;
  InstructionId::IdType length;

  ParameterTypeId::IdType param_types_count;
  union {
    Type soo_buf[kBlockParameterTypesSooThreshold];
    ParameterTypeId storage_id;
  } param_types;

  inline constexpr InstructionId end() const {
    return InstructionId(head_id.id + length);
  }
};

using BlockId = base::Id<Block, IdBaseType>;

}  // namespace ir
