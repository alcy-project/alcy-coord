// Copyright 2026 pugur
// This source code is licensed under the Apache License, Version 2.0 with LLVM
// Exceptions which can be found in the LICENSE file.

#pragma once

#include <utility>

#include "ir/storage.h"

namespace ir {

class Builder {
 public:
  Builder() = default;
  ~Builder() = default;

  Builder(const Builder&) = delete;
  Builder& operator=(const Builder&) = delete;

  Builder(Builder&&) noexcept = default;
  Builder& operator=(Builder&&) noexcept = default;

  void init();

  [[nodiscard]] inline Storage take() && { return std::move(storage_); }

  void start_block();
  void end_block();

 private:
  Storage storage_;
};

}  // namespace ir

