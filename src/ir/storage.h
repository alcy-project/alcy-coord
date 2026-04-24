// Copyright 2026 pugur
// This source code is licensed under the Apache License, Version 2.0 with LLVM
// Exceptions which can be found in the LICENSE file.

#pragma once

#include <memory>
#include <string>
#include <vector>

#include "fpag/base/numeric.h"
#include "ir/block.h"
#include "ir/function_meta.h"
#include "ir/imm.h"
#include "ir/instruction.h"

namespace ir {

// instance per function
class Storage {
 public:
  Storage() = default;
  ~Storage() = default;

  Storage(const Storage&) = delete;
  Storage& operator=(const Storage&) = delete;

  Storage(Storage&&) noexcept = default;
  Storage& operator=(Storage&&) noexcept = default;

  inline void init_function_meta(FunctionMeta function_meta) {
    function_meta_ = function_meta;
  }

  inline u32 add_instruction(Instruction instruction) {
    insts_.emplace_back(instruction);
    return static_cast<u32>(insts_.size());
  }
  inline BlockId add_block(Block block) {
    blocks_.emplace_back(block);
    return static_cast<u32>(blocks_.size() - 1);
  }
  // TODO: use local hash table or concurrent global hash table to eliminate
  // duplicated values
  inline ImmIId add_imm_i(u64 i) {
    imm_i_.emplace_back(i);
    return static_cast<u32>(imm_i_.size() - 1);
  }
  inline ImmFId add_imm_f(f64 f) {
    imm_f_.emplace_back(f);
    return static_cast<u32>(imm_f_.size() - 1);
  }

  inline const FunctionMeta function_meta() const { return function_meta_; }
  inline const Instruction instruction(usize index) const {
    return insts_[index];
  }
  inline const Block block(usize index) const { return blocks_[index]; }
  inline u64 imm_i(usize index) const { return imm_i_[index]; }
  inline f64 imm_f(usize index) const { return imm_f_[index]; }

  inline usize num_instructions() const { return insts_.size(); }
  inline usize num_blocks() const { return blocks_.size(); }

  std::string dump() const;

 private:
  FunctionMeta function_meta_;
  std::vector<Block> blocks_;
  std::vector<u64> imm_i_;
  std::vector<f64> imm_f_;
  std::vector<Instruction> insts_;
};

}  // namespace ir
