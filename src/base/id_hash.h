// Copyright 2026 pugur
// This source code is licensed under the Apache License, Version 2.0 with LLVM
// Exceptions which can be found in the LICENSE file.

#pragma once

#include <functional>

#include "base/id.h"
#include "fpag/base/numeric.h"

namespace std {

template <typename T, typename I>
struct hash<base::Id<T, I>> {
  usize operator()(const base::Id<T, I>& id) const noexcept {
    return std::hash<I>{}(id.value);
  }
};

}  // namespace std

