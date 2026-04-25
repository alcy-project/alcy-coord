// Copyright 2026 pugur
// This source code is licensed under the Apache License, Version 2.0 with LLVM
// Exceptions which can be found in the LICENSE file.

#pragma once

#include "base/id.h"
#include "fpag/base/numeric.h"
#include "ir/common.h"
#include "ir/type.h"

namespace ir {

struct Function {
  BlockId head_id;
  BlockId::IdType length;
  Type return_type;

  inline constexpr BlockId end() const { return BlockId(head_id.id + length); }
};

using FunctionId = base::Id<Function, IdBaseType>;

}  // namespace ir
