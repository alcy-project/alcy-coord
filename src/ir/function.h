// Copyright 2026 pugur
// This source code is licensed under the Apache License, Version 2.0 with LLVM
// Exceptions which can be found in the LICENSE file.

#pragma once

#include "base/id.h"
#include "fpag/base/numeric.h"
#include "fpag/str/string_pool_id.h"
#include "ir/common.h"
#include "ir/type.h"

namespace ir {

struct FunctionMeta {
  Type return_type;
  str::StringPoolId name;
};

struct Function {
  FunctionMeta meta;

  BlockId head_id;
  BlockId::IdType length;

  inline constexpr BlockId end() const { return BlockId(head_id.id + length); }
};

}  // namespace ir
