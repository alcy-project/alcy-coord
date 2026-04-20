// Copyright 2026 pugur
// This source code is licensed under the Apache License, Version 2.0 with LLVM
// Exceptions which can be found in the LICENSE file.

#pragma once

#include "fpag/base/numeric.h"

namespace ir {

enum class OpCode : u8 {
  Noop,

  ConstInt,
  ConstFloat,
  ConstPool,

  Alloca,
  Load,
  Store,
  GetElementPtr,
  ExtractValue,
  InsertValue,
  MemCopy,
  MemSet,

  IAdd,
  ISub,
  IMul,
  IDiv,  // signed
  UDiv,  // unsigned
  IRem,  // signed
  URem,  // unsigned

  IAddWrapping,
  ISubWrapping,
  IMulWrapping,
  IAddSaturating,
  ISubSaturating,
  IMulSaturating,

  And,
  Or,
  Xor,
  Shl,
  LShr,
  AShr,
  Not,
  BitReverse,
  PopCount,

  FAdd,
  FSub,
  FMul,
  FDiv,
  FRem,
  FNeg,
  FAbs,

  ICmpEq,
  ICmpNe,
  ICmpSlt,
  ICmpSle,
  ICmpUlt,
  ICmpUle,

  FCmpEq,
  FCmpNe,
  FCmpLt,
  FCmpLe,
  FCmpOEq,
  FCmpONe,
  FCmpOLt,
  FCmpOLe,
  FCmpUEq,
  FCmpUNe,
  FCmpULt,
  FCmpULe,
  FCmpOrd,
  FCmpUno,

  Cast,

  Select,

  Br,
  CondBr,
  Switch,
  Call,
  Ret,
  Unreachable,

  AtomicLoad,
  AtomicStore,
  AtomicRMW,
  AtomicCmpXchg,
  Fence,

  Move,
  Drop,
  BorrowBegin,
  BorrowEnd,

  VecSplat,
  VecExtract,
  VecInsert,
  VecReduce,
};

}  // namespace ir
