// Copyright 2026 pugur
// This source code is licensed under the Apache License, Version 2.0 with LLVM
// Exceptions which can be found in the LICENSE file.

#pragma once

#include "fpag/base/numeric.h"

namespace ir {

enum class Type : u8 {
  Void,  // For function return type
  I1,    // bool
  I8,
  I16,
  I32,
  I64,
  // I128,
  // U8,
  // U16,
  // U32,
  // U64,
  // U128,
  // F16,
  F32,
  F64,
  // F128,
  Ptr,     // Opaque pointer
  Ref,     // Immutable reference (region tracked)
  MutRef,  // Mutable reference  (exclusive, region tracked)

  Function,
  Block,  // SSA block
};

constexpr const char* type_to_str(Type type) {
  using T = Type;
  switch (type) {
    case T::Void: return "Void";
    case T::I1: return "I1";
    case T::I8: return "I8";
    case T::I16: return "I16";
    case T::I32: return "I32";
    case T::I64: return "I64";
    // case T::I128: return "I128";
    // case T::U8: return "U8";
    // case T::U16: return "U16";
    // case T::U32: return "U32";
    // case T::U64: return "U64";
    // case T::U128: return "U128";
    // case T::F16: return "F16";
    case T::F32: return "F32";
    case T::F64: return "F64";
    // case T::F128: return "F128";
    case T::Ptr: return "Ptr";
    case T::Ref: return "Ref";
    case T::MutRef: return "MutRef";

    case T::Function: return "Function";
    case T::Block: return "Block";
  }
}

}  // namespace ir
