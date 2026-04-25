// Copyright 2026 pugur
// This source code is licensed under the Apache License, Version 2.0 with LLVM
// Exceptions which can be found in the LICENSE file.

#include "codegen_llvm/llvm_ir_emitter.h"

#include <memory>
#include <string>
#include <utility>
#include <vector>

#include "fpag/mem/concurrent_arena.h"
#include "ir/builder.h"
#include "ir/opcode.h"
#include "ir/register.h"
#include "ir/type.h"
#include "llvm/IR/Module.h"

#ifdef CHECK
#undef CHECK
#endif

#include "catch2/catch_all.hpp"

namespace codegen {

TEST_CASE("LlvmIrEmitter: simple addition and execution", "[codegen][llvm]") {
  llvm::LLVMContext context;
  std::unique_ptr<llvm::Module> module =
      std::make_unique<llvm::Module>("llvm_ir_gen_test", context);

  // calculate 10 + 20 and return
  // reg[0] = 10;
  // reg[1] = 20;
  // reg[2] = reg[0] + reg[1];
  // ret reg[2]
  // mock_ir.add_instruction({ir::Opcode::ConstInt, {}, 0, 10, 0});
  // mock_ir.add_instruction({ir::Opcode::ConstInt, {}, 1, 20, 0});
  // mock_ir.add_instruction({ir::Opcode::IAdd, {}, 2, 0, 1});
  // mock_ir.add_instruction({ir::Opcode::Ret, {}, 0, 2, 0});

  mem::ConcurrentArena arena;
  arena.reserve(4 * 1024 * 1024);  // 4 MiB
  ir::Builder b;
  b.init(&arena, {.fn_name_id = {.offset = 0, .length = 0},
                  .return_type = ir::Type::I32});

  b.inst(ir::Opcode::ConstInt, 0, b.imm_i_operand(10, ir::Type::I32),
         b.invalid());
  b.inst(ir::Opcode::ConstInt, 1, b.imm_i_operand(20, ir::Type::I32),
         b.invalid());
  b.inst(ir::Opcode::IAdd, 2, b.reg_operand(0), b.reg_operand(1));
  b.inst(ir::Opcode::Ret, 0, b.reg_operand(2), b.invalid());

  b.build();
  const auto mock_ir = std::move(b).take();

  CHECK(mock_ir->num_instructions() == 4);
  CHECK(mock_ir->num_blocks() == 1);

  // DLOG("mock_ir: {}", mock_ir->dump());

  LlvmIrEmitter generator;
  generator.init(&context, module.get());

  llvm::Function* function = generator.generate(*mock_ir);
  REQUIRE(function);

  SECTION("structure validation") {
    // Check if func has one basic block
    REQUIRE(function->size() == 1);

    auto it = function->getEntryBlock().begin();
    REQUIRE(it.isValid());

    // Add must not be here because of constant foldings by
    // IRBuilder<ConstantFolder>
    REQUIRE(it->getOpcode() == llvm::Instruction::Ret);
    // on_debug(module->print(llvm::outs(), nullptr));
  }

  // SECTION("jit execution") {
  //   llvm::InitializeNativeTarget();
  //   llvm::InitializeNativeTargetAsmPrinter();

  //   std::string error_buf;
  //   llvm::ExecutionEngine* ee =
  //       llvm::EngineBuilder(std::move(module)).setErrorStr(&error_buf).create();

  //   if (!ee) {
  //     FAIL("failed to create ExecutionEngine: " << error_buf);
  //   }

  //   // execute function
  //   std::vector<llvm::GenericValue> no_args;
  //   llvm::GenericValue result = ee->runFunction(function, no_args);

  //   // check 10 + 20 = 30
  //   CHECK(result.IntVal.getSExtValue() == 30);

  //   delete ee;
  // }
}

}  // namespace codegen
