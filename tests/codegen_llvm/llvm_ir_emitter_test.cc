// Copyright 2026 pugur
// This source code is licensed under the Apache License, Version 2.0 with LLVM
// Exceptions which can be found in the LICENSE file.

#include "codegen_llvm/llvm_ir_emitter.h"

#include <memory>
#include <string>
#include <utility>

#include "catch2/catch_all.hpp"
#include "fpag/str/string_interner.h"
#include "ir/common.h"
#include "ir/external_function.h"
#include "ir/opcode.h"
#include "ir/operand.h"
#include "ir/storage.h"
#include "ir/type.h"
#include "llvm/IR/IRBuilder.h"
#include "llvm/IR/LLVMContext.h"
#include "llvm/IR/Module.h"
#include "llvm/IR/Verifier.h"

namespace codegen_llvm {

TEST_CASE("simple failing", "[codegen_llvm]") {
  std::unique_ptr<ir::Storage> storage = std::make_unique<ir::Storage>();

  str::StringInterner interner(4096);

  const str::StringPoolId main_str = interner.intern("main");
  const str::StringPoolId puts_str = interner.intern("puts");

  const ir::ImmutableId imm_1 =
      storage->add_immutable({.type = ir::Type::I32, .data = {.i32 = 1}});

  const ir::ExternalFunctionId func_puts = storage->add_external_function({
      .meta = {.return_type = ir::Type::I32,
               .parameter_count = 1,
               .parameter_types = {ir::Type::Ptr},
               .name = puts_str},
      .calling_conv = ir::CallingConvention::C,
  });

  // reg 0 := 1 - 1 = 0
  const ir::InstructionId head = storage->add_instruction({
      .op = ir::Opcode::IntSub,
      .flags = {.is_var_len = false},
      .dst = ir::RegisterId(0),
      .lhs = {.tag = ir::OperandTag::Immutable,
              .type = ir::Type::I32,
              .data = {.immutable_id = imm_1}},
      .rhs = {.tag = ir::OperandTag::Immutable,
              .type = ir::Type::I32,
              .data = {.immutable_id = imm_1}},
  });
  storage->add_register({.type = ir::Type::I32, .def_idx = head});

  storage->add_instruction({
      .op = ir::Opcode::Call,
      .flags = {.is_var_len = false},
      .dst = ir::RegisterId(1),
      .lhs = {.tag = ir::OperandTag::ExternalFunction,
              .type = ir::Type::Function,
              .data = {.external_function_id = func_puts}},
      // TODO
      // .rhs = {.tag = ir::OperandTag::Immutable,
      //         .type = ir::Type::Ptr,
      //         .data = {.immutable_id = imm_str}},
      .rhs = ir::kInvalidOperand,
  });

  const ir::InstructionId end = storage->add_instruction({
      .op = ir::Opcode::Ret,
      .flags = {.is_var_len = false},
      .dst = ir::RegisterId(2),
      .lhs = {.tag = ir::OperandTag::Register,
              .type = ir::Type::I32,
              .data = {.register_id = ir::RegisterId(0)}},
      .rhs = ir::kInvalidOperand,
  });

  const ir::BlockId block = storage->add_block({
      .head_id = head,
      .length = end.id - head.id + 1,
  });

  storage->add_function({
      .meta = {.return_type = ir::Type::I32,
               .parameter_count = 0,
               .parameter_types = {},
               .name = main_str},
      .head_id = block,
      .length = 1,
  });

  llvm::LLVMContext context;
  std::unique_ptr<llvm::Module> module =
      std::make_unique<llvm::Module>("llvm_ir_emitter_test", context);
  LlvmIrEmitter emitter(module.get(), std::move(storage), &interner);

  std::move(emitter).emit();

  CHECK(!llvm::verifyModule(*module));

  std::string ir_str;
  llvm::raw_string_ostream os(ir_str);
  module->print(os, nullptr);

  // logging::SyncLogger& logger = test_logger();
  // logger.debug("LLVM IR dump:\n{}", os.str());
}

}  // namespace codegen_llvm
