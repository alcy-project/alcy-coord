// Copyright 2026 pugur
// This source code is licensed under the Apache License, Version 2.0 with LLVM
// Exceptions which can be found in the LICENSE file.

#pragma once

#include <memory>
#include <string>
#include <utility>
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

struct StorageState {
  template <typename T>
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

  Functions functions;
  Blocks blocks;
  Instructions instrs;
  Immutables immutables;
  Registers registers;
  ExternalFunctions external_functions;
  Operands operands;
  ParameterTypes parameter_types;
};

class Storage {
 public:
  explicit Storage(StorageState&& state) : state_(std::move(state)) {}
  ~Storage() = default;

  Storage(const Storage&) = delete;
  Storage& operator=(const Storage&) = delete;

  Storage(Storage&&) noexcept = default;
  Storage& operator=(Storage&&) noexcept = default;

  inline const StorageState& state() const { return state_; }

  inline const StorageState::Functions& functions() const {
    return state_.functions;
  }
  inline const StorageState::Blocks& blocks() const { return state_.blocks; }
  inline const StorageState::Instructions& instrs() const {
    return state_.instrs;
  }
  inline const StorageState::Immutables& immutables() const {
    return state_.immutables;
  }
  inline const StorageState::Registers& registers() const {
    return state_.registers;
  }
  inline const StorageState::ExternalFunctions& external_functions() const {
    return state_.external_functions;
  }
  inline const StorageState::Operands& operands() const {
    return state_.operands;
  }
  inline const StorageState::ParameterTypes& parameter_types() const {
    return state_.parameter_types;
  }

 private:
  StorageState state_;
};

}  // namespace ir
