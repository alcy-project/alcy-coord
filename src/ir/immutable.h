// Copyright 2026 pugur
// This source code is licensed under the Apache License, Version 2.0 with LLVM
// Exceptions which can be found in the LICENSE file.

#pragma once

#include "base/debug/check.h"
#include "base/debug/fatal.h"
#include "fpag/base/numeric.h"
#include "ir/type.h"
#include "ir/type_util.h"

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

  inline u64 as_u64_integer() const {
    DCHECK_MSG(is_integer_type(type),
               "called as_u64_integer with not integer type");
    switch (type) {
      case Type::I1: return static_cast<u64>(data.i1);
      case Type::I8: return static_cast<u64>(data.i8);
      case Type::I16: return static_cast<u64>(data.i16);
      case Type::I32: return static_cast<u64>(data.i32);
      case Type::I64: return static_cast<u64>(data.i64);
      default: UNREACHABLE();
    }
  }

  inline f64 as_f64_fp() const {
    DCHECK_MSG(is_float_type(type),
               "called as_f64_fp with not floating point type");
    switch (type) {
      case Type::F32: return static_cast<f64>(data.f32);
      case Type::F64: return static_cast<f64>(data.f64);
      default: UNREACHABLE();
    }
  }
};

}  // namespace ir
