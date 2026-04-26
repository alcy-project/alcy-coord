// Copyright 2026 pugur
// This source code is licensed under the Apache License, Version 2.0 with LLVM
// Exceptions which can be found in the LICENSE file.

#pragma once

#include "fpag/base/numeric.h"
#include "ir/type.h"

namespace ir {

struct Immutable {
  Type type;
  union {
    bool i1;
    i8 i8;
    i16 i16;
    i32 i32;
    i64 i64;
    f32 f32;
    f64 f64;
    u64 ptr;
    u64 mutptr;
    u64 ref;
    u64 mutref;
  } data;
};

}  // namespace ir
