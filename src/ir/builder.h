// Copyright 2026 pugur
// This source code is licensed under the Apache License, Version 2.0 with LLVM
// Exceptions which can be found in the LICENSE file.

#pragma once

#include <utility>

#include "base/debug/check.h"
#include "build/build_config.h"
#include "fpag/mem/arena_ptr.h"
#include "fpag/mem/concurrent_arena.h"
#include "ir/block.h"
#include "ir/function_meta.h"
#include "ir/imm.h"
#include "ir/instruction.h"
#include "ir/opcode.h"
#include "ir/register.h"
#include "ir/storage.h"
#include "ir/type.h"

namespace ir {

class Builder {
 public:
  Builder() = default;
  ~Builder() = default;

  Builder(const Builder&) = delete;
  Builder& operator=(const Builder&) = delete;

  Builder(Builder&&) noexcept = default;
  Builder& operator=(Builder&&) noexcept = default;

  void init(mem::ConcurrentArena* arena, FunctionMeta function_meta) {
    DCHECK(!initialized_ && !built_);
    storage_ = arena->create_managed<Storage>();
    storage_->init_function_meta(function_meta);
    DCHECK(storage_);
#if BUILD_FLAG(IS_DEBUG)
    initialized_ = true;
#endif
  }

  void build() {
    DCHECK(initialized_ && !built_);
    punctuate_block();
    built_ = true;
  }

  [[nodiscard]] inline mem::ArenaUniquePtr<Storage> take() && {
    DCHECK(initialized_ && built_);
    return std::move(storage_);
  }

  static inline constexpr Operand reg_operand(RegisterId reg,
                                              Type type = Type::Void) {
    return Operand{
        .tag = OperandTag::Register, .type = type, .data = Operand::Data(reg)};
  }
  static inline constexpr Operand block_operand(BlockId block) {
    return Operand{.tag = OperandTag::Block,
                   .type = Type::Void,
                   .data = Operand::Data(block)};
  }
  inline Operand imm_i_operand(u64 i, Type type = Type::Void) {
    DCHECK(initialized_ && !built_);
    const ImmIId id = storage_->add_imm_i(i);
    return Operand{
        .tag = OperandTag::ImmI, .type = type, .data = Operand::Data(id)};
  }
  inline Operand imm_f_operand(f64 f, Type type = Type::Void) {
    DCHECK(initialized_ && !built_);
    const ImmFId id = storage_->add_imm_f(f);
    return Operand{
        .tag = OperandTag::ImmF, .type = type, .data = Operand::Data(id)};
  }

  static inline constexpr Operand invalid() {
    return reg_operand(kInvalidRegisterId, Type::Void);
  }

  void inst(OpCode op, RegisterId dst, Operand lhs, Operand rhs) {
    storage_->add_instruction(
        {.op = op, .flags = {}, .dst = dst, .lhs = lhs, .rhs = rhs});
  }

  // void start_block();
  // void end_block();
  void punctuate_block() {
    DCHECK(initialized_ && !built_);
    DCHECK_GT(static_cast<u32>(storage_->num_instructions()), block_start_idx_);
    const u32 len =
        static_cast<u32>(storage_->num_instructions()) - block_start_idx_;
    storage_->add_block(Block{.head = block_start_idx_, .length = len});
    block_start_idx_ += len + 1;
  }

 private:
  mem::ArenaUniquePtr<Storage> storage_ = nullptr;
  u32 block_start_idx_ = 0;

  bool initialized_ : 1 = false;
  bool built_ : 1 = false;
};

}  // namespace ir
