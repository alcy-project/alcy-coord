// Copyright 2026 pugur
// This source code is licensed under the Apache License, Version 2.0 with LLVM
// Exceptions which can be found in the LICENSE file.

#pragma once

#include "base/id.h"
#include "fpag/base/numeric.h"
#include "ir/common.h"

namespace ir {

struct Block {
  InstructionId head_id;
  InstructionId::IdType length;

  inline constexpr InstructionId end() const {
    return InstructionId(head_id.id + length);
  }

  // TODO: add block parameters(instead of phi)
};

using BlockId = base::Id<Block, IdBaseType>;

}  // namespace ir
