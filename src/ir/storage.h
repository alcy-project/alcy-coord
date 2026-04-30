// Copyright 2026 pugur
// This source code is licensed under the Apache License, Version 2.0 with LLVM
// Exceptions which can be found in the LICENSE file.

#pragma once

#include <memory>
#include <string>
#include <vector>

#include "base/vec.h"
#include "fpag/base/numeric.h"
#include "ir/block.h"
#include "ir/common.h"
#include "ir/external_function.h"
#include "ir/function.h"
#include "ir/immutable.h"
#include "ir/instruction.h"
#include "ir/register.h"

namespace ir {

class Storage {
 public:
  template <typename T>
  // using Alloc = base::SingleThreadPageAllocator<T>;
  using Alloc = std::allocator<T>;

  using Functions = base::Vec<Function, FunctionId, Alloc<Function>>;
  using Blocks = base::Vec<Block, BlockId, Alloc<Block>>;
  using Instructions =
      base::Vec<Instruction, InstructionId, Alloc<Instruction>>;
  using Immutables = base::Vec<Immutable, ImmutableId, Alloc<Immutable>>;
  using Registers = base::Vec<Register, RegisterId, Alloc<Register>>;
  using ExternalFunctions =
      base::Vec<ExternalFunction, ExternalFunctionId, Alloc<ExternalFunction>>;
  using Operands = base::Vec<Operand, OperandId, Alloc<Operand>>;
  using ParameterTypes = base::Vec<Type, ParameterTypeId, Alloc<Type>>;

  Storage() = default;
  ~Storage() = default;

  Storage(const Storage&) = delete;
  Storage& operator=(const Storage&) = delete;

  Storage(Storage&&) noexcept = default;
  Storage& operator=(Storage&&) noexcept = default;

  inline FunctionId add_function(Function&& function) {
    return functions_.emplace_back(function);
  }

  inline BlockId add_block(Block&& block) {
    return blocks_.emplace_back(block);
  }

  inline InstructionId add_instruction(Instruction&& instruction) {
    return instrs_.emplace_back(instruction);
  }

  inline ImmutableId add_immutable(Immutable&& immutable) {
    return immutables_.emplace_back(immutable);
  }

  inline RegisterId add_register(Register&& reg) {
    return registers_.emplace_back(reg);
  }
  inline ExternalFunctionId add_external_function(ExternalFunction&& ex_func) {
    return external_functions_.emplace_back(ex_func);
  }
  inline OperandId add_operand(Operand&& op) {
    return operands_.emplace_back(op);
  }
  inline ParameterTypeId add_parameter_type(Type&& type) {
    return parameter_types_.emplace_back(type);
  }

  inline const Functions& functions() const { return functions_; }
  inline const Instructions& instructions() const { return instrs_; }
  inline const Blocks& blocks() const { return blocks_; }
  inline const Immutables& immutables() const { return immutables_; }
  inline const Registers& registers() const { return registers_; }
  inline const ExternalFunctions& external_functions() const {
    return external_functions_;
  }
  inline const Operands& operands() const { return operands_; }
  inline const ParameterTypes& parameter_types() const {
    return parameter_types_;
  }

  // std::string dump() const;

 private:
  Functions functions_;
  Blocks blocks_;
  Instructions instrs_;
  Immutables immutables_;
  Registers registers_;
  ExternalFunctions external_functions_;
  Operands operands_;
  ParameterTypes parameter_types_;
};

}  // namespace ir
