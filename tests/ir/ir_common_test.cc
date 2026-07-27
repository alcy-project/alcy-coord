// Copyright 2026 pugur
// This source code is licensed under the Apache License, Version 2.0 with LLVM
// Exceptions which can be found in the LICENSE file.

#include "catch2/catch_test_macros.hpp"
#include "ir/block.h"
#include "ir/block_param.h"
#include "ir/common.h"
#include "ir/external_function.h"
#include "ir/function.h"
#include "ir/immutable.h"
#include "ir/instruction.h"
#include "ir/instruction_flags.h"
#include "ir/opcode.h"
#include "ir/operand.h"
#include "ir/register.h"
// #include "ir/storage.h"
// #include "ir/storage_builder.h"
#include "ir/type.h"

namespace ir {

TEST_CASE("Static assertion for IR elements", "[ir]") {
  STATIC_CHECK(sizeof(Block) == 16);
  STATIC_CHECK(sizeof(BlockParam) == 8);

  STATIC_CHECK(sizeof(ExternalFunction) == 40);
  STATIC_CHECK(sizeof(Function) == 40);
  STATIC_CHECK(sizeof(FunctionMeta) == 32);

  STATIC_CHECK(sizeof(Immutable) == 24);
  STATIC_CHECK(sizeof(Instruction) == 16);
  STATIC_CHECK(sizeof(InstructionFlags) == 1);
  STATIC_CHECK(sizeof(Opcode) == 1);
  STATIC_CHECK(sizeof(Operand) == 8);
  STATIC_CHECK(sizeof(Register) == 8);
  STATIC_CHECK(sizeof(Type) == 1);

  // STATIC_CHECK(sizeof(StorageState) == 216);
  // STATIC_CHECK(sizeof(Storage) == 216);
  // STATIC_CHECK(sizeof(StorageBuilder) == 216);
}

}  // namespace ir

