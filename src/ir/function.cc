// Copyright 2026 pugur
// This source code is licensed under the Apache License, Version 2.0 with LLVM
// Exceptions which can be found in the LICENSE file.

#include "ir/function.h"

namespace ir {

static_assert(sizeof(FunctionMeta) == 24);
static_assert(sizeof(Function) == 32);

}  // namespace ir
