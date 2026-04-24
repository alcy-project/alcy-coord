// Copyright 2026 pugur
// This source code is licensed under the Apache License, Version 2.0 with LLVM
// Exceptions which can be found in the LICENSE file.

#pragma once

#include "fpag/base/numeric.h"

namespace ir {

enum class Type : u8 {
  Void,
  I1,  // bool
  I8,
  I16,
  I32,
  I64,
  I128,
  // U8,
  // U16,
  // U32,
  // U64,
  // U128,
  F16,
  F32,
  F64,
  // F128,
  Ptr,     // opaque pointer
  Ref,     // immutable reference — region tracked
  MutRef,  // mutable reference  — exclusive, region tracked
};

}  // namespace ir
