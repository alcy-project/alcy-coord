// Copyright 2026 pugur
// This source code is licensed under the Apache License, Version 2.0 with LLVM
// Exceptions which can be found in the LICENSE file.

#pragma once

namespace llvm {

struct Module;
struct Type;
struct Function;
struct BasicBlock;
struct Value;
struct Constant;

struct ConstantFolder;
struct IRBuilderDefaultInserter;

template <typename T, typename Inserter>
struct IRBuilder;

}  // namespace llvm

