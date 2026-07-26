// Copyright 2026 pugur
// This source code is licensed under the Apache License, Version 2.0 with LLVM
// Exceptions which can be found in the LICENSE file.

#pragma once

#include <memory>
#include <utility>

#include "ir/storage.h"

namespace ir {

class StorageBuilder {
 public:
  StorageBuilder() = default;
  explicit StorageBuilder(StorageState&& state) : state_(std::move(state)) {}

  ~StorageBuilder() = default;

  StorageBuilder(const StorageBuilder&) = delete;
  StorageBuilder& operator=(const StorageBuilder&) = delete;

  StorageBuilder(StorageBuilder&&) noexcept = default;
  StorageBuilder& operator=(StorageBuilder&&) noexcept = default;

  inline StorageBuilder& functions(StorageState::Functions&& functions) {
    state_.functions = std::move(functions);
    return *this;
  }
  inline StorageBuilder& blocks(StorageState::Blocks&& blocks) {
    state_.blocks = std::move(blocks);
    return *this;
  }
  inline StorageBuilder& instrs(StorageState::Instructions&& instrs) {
    state_.instrs = std::move(instrs);
    return *this;
  }
  inline StorageBuilder& immutables(StorageState::Immutables&& immutables) {
    state_.immutables = std::move(immutables);
    return *this;
  }
  inline StorageBuilder& registers(StorageState::Registers&& registers) {
    state_.registers = std::move(registers);
    return *this;
  }
  inline StorageBuilder& external_functions(
      StorageState::ExternalFunctions&& external_functions) {
    state_.external_functions = std::move(external_functions);
    return *this;
  }
  inline StorageBuilder& operands(StorageState::Operands&& operands) {
    state_.operands = std::move(operands);
    return *this;
  }
  inline StorageBuilder& parameter_types(
      StorageState::ParameterTypes&& parameter_types) {
    state_.parameter_types = std::move(parameter_types);
    return *this;
  }

  inline FunctionId function(Function function) {
    return state_.functions.emplace_back(function);
  }
  inline BlockId block(Block block) {
    return state_.blocks.emplace_back(block);
  }
  inline InstructionId instr(Instruction instr) {
    return state_.instrs.emplace_back(instr);
  }
  inline ImmutableId immutable(Immutable immutable) {
    return state_.immutables.emplace_back(immutable);
  }
  inline RegisterId reg(Register reg) {
    return state_.registers.emplace_back(reg);
  }
  inline ExternalFunctionId external_function(
      ExternalFunction external_function) {
    return state_.external_functions.emplace_back(external_function);
  }
  inline OperandId operand(Operand operand) {
    return state_.operands.emplace_back(operand);
  }
  inline ParameterTypeId parameter_type(Type parameter_type) {
    return state_.parameter_types.emplace_back(parameter_type);
  }

  inline Storage build() && { return Storage(std::move(state_)); }

  inline std::unique_ptr<Storage> build_unique() && {
    return std::make_unique<Storage>(std::move(*this).build());
  }

 private:
  StorageState state_;
};

}  // namespace ir
