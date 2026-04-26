// Copyright 2026 pugur
// This source code is licensed under the Apache License, Version 2.0 with LLVM
// Exceptions which can be found in the LICENSE file.

#pragma once

#include "base/id.h"
#include "fpag/base/numeric.h"

namespace ir {

using IdBaseType = base::IdBaseType;

struct Function;
using FunctionId = base::Id<Function, IdBaseType>;

struct Block;
using BlockId = base::Id<Block, IdBaseType>;

struct Immutable;
using ImmutableId = base::Id<Immutable, IdBaseType>;

struct Register;
using RegisterId = base::Id<Register, IdBaseType>;

struct Instruction;
using InstructionId = base::Id<Instruction, IdBaseType>;

struct ExternalFunction;
using ExternalFunctionId = base::Id<ExternalFunction, IdBaseType>;

struct Operand;

}  // namespace ir
