// Copyright 2026 pugur
// This source code is licensed under the Apache License, Version 2.0 with LLVM
// Exceptions which can be found in the LICENSE file.

#pragma once

#include "fpag/str/string_pool_id.h"
#include "ir/type.h"

namespace ir {

struct FunctionMeta {
  str::StringPoolId fn_name_id;
  Type return_type = Type::Void;
};

}  // namespace ir
