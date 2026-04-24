// Copyright 2026 pugur
// This source code is licensed under the Apache License, Version 2.0 with LLVM
// Exceptions which can be found in the LICENSE file.

#pragma once

#include "fpag/base/numeric.h"
#include "ir/block.h"
#include "ir/imm.h"
#include "ir/opcode.h"
#include "ir/register.h"
#include "ir/type.h"

namespace ir {

struct InstuctionFlags {
  bool some_flag : 1 = false;
};

enum class OperandTag : u8 {
  Register,
  Block,
  ImmI,
  ImmF,
  Unknown,
};

struct Operand {
  OperandTag tag;
  Type type = Type::Void;

  union Data {
    RegisterId reg;
    BlockId block;
    ImmIId imm_i;
    ImmFId imm_f;

    constexpr Data() : reg(0) {}
    explicit constexpr Data(RegisterId r) { reg = r; }
    explicit constexpr Data(BlockId b) { block = b; }
    explicit constexpr Data(ImmIId i) { imm_i = i; }
    explicit constexpr Data(ImmFId f) { imm_f = f; }
  } data;
};

struct Instruction {
  OpCode op;
  InstuctionFlags flags;
  // u16 _padding;

  // u32 dst;
  // u32 lhs;
  // u32 rhs;

  RegisterId dst;
  Operand lhs;
  Operand rhs;
};

}  // namespace ir
