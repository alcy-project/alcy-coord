// Copyright 2026 pugur
// This source code is licensed under the Apache License, Version 2.0 with LLVM
// Exceptions which can be found in the LICENSE file.

#pragma once

#include "fpag/base/numeric.h"

namespace ir {

enum class Opcode : u8 {
  Noop,

  Alloca,
  Load,
  Store,
  GetElementPtr,
  ExtractValue,
  InsertValue,

  IntAdd,
  IntSub,
  IntMul,
  IntDiv,   // Signed devision
  UintDiv,  // Unsigned devision
  IntRem,   // Signed remainder
  UintRem,  // Unsigned remainder

  And,
  Or,
  Xor,
  ShiftLeft,
  ArithmeticShiftRight,
  LogicalShiftRight,
  Not,
  BitReverse,

  Eq,
  Ne,
  Le,
  Lt,
  Ge,
  Gt,

  TypeCast,

  Select,

  Br,
  CondBr,
  Switch,
  Call,
  Ret,
  Unreachable,

  AtomicLoad,
  AtomicStore,
  AtomicRmw,
  AtomicCompareExchange,
  Fence,

  Move,
  Drop,
  // BorrowBegin,
  // BorrowEnd,

  // VecSplat,
  // VecExtract,
  // VecInsert,
  // VecReduce,
};

}  // namespace ir
