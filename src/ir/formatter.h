// Copyright 2026 pugur
// This source code is licensed under the Apache License, Version 2.0 with LLVM
// Exceptions which can be found in the LICENSE file.

#pragma once

#include <string_view>

#include "fpag/str/format_util.h"
#include "ir/opcode.h"

namespace ir {

constexpr std::string_view format_as(const Opcode c) {
  switch (c) {
    case ir::Opcode::Noop: return "Noop";

    case ir::Opcode::Alloca: return "Alloca";
    case ir::Opcode::Load: return "Load";
    case ir::Opcode::Store: return "Store";
    case ir::Opcode::GetElementPtr: return "GetElementPtr";
    case ir::Opcode::ExtractValue: return "ExtractValue";
    case ir::Opcode::InsertValue: return "InsertValue";

    case ir::Opcode::IntAdd: return "IntAdd";
    case ir::Opcode::IntSub: return "IntSub";
    case ir::Opcode::IntMul: return "IntMul";
    case ir::Opcode::IntDiv: return "IntDiv";
    case ir::Opcode::UintDiv: return "UintDiv";
    case ir::Opcode::IntRem: return "IntRem";
    case ir::Opcode::UintRem: return "UintRem";

    case ir::Opcode::And: return "And";
    case ir::Opcode::Or: return "Or";
    case ir::Opcode::Xor: return "Xor";
    case ir::Opcode::ShiftLeft: return "ShiftLeft";
    case ir::Opcode::ArithmeticShiftRight: return "ArithmeticShiftRight";
    case ir::Opcode::LogicalShiftRight: return "LogicalShiftRight";
    case ir::Opcode::Not: return "Not";
    case ir::Opcode::BitReverse: return "BitReverse";

    case ir::Opcode::Eq: return "Eq";
    case ir::Opcode::Ne: return "Ne";
    case ir::Opcode::Le: return "Le";
    case ir::Opcode::Lt: return "Lt";
    case ir::Opcode::Ge: return "Ge";
    case ir::Opcode::Gt: return "Gt";

    case ir::Opcode::TypeCast: return "TypeCast";

    case ir::Opcode::Select: return "Select";

    case ir::Opcode::Br: return "Br";
    case ir::Opcode::CondBr: return "CondBr";
    case ir::Opcode::Switch: return "Switch";
    case ir::Opcode::Call: return "Call";
    case ir::Opcode::Ret: return "Ret";
    case ir::Opcode::Unreachable: return "Unreachable";

    case ir::Opcode::AtomicLoad: return "AtomicLoad";
    case ir::Opcode::AtomicStore: return "AtomicStore";
    case ir::Opcode::AtomicRmw: return "AtomicRmw";
    case ir::Opcode::AtomicCompareExchange: return "AtomicCompareExchange";

    case ir::Opcode::Fence: return "Fence";

    case ir::Opcode::Move: return "Move";
    case ir::Opcode::Drop:
      return "Drop";

      // case ir::Opcode::BorrowBegin: return "BorrowBegin";
      // case ir::Opcode::BorrowEnd: return "BorrowEnd";

      // case ir::Opcode::VecSplat: return "VecSplat";
      // case ir::Opcode::VecExtract: return "VecExtract";
      // case ir::Opcode::VecInsert: return "VecInsert";
      // case ir::Opcode::VecReduce: return "VecReduce";
  }
}

}  // namespace ir

