// Copyright 2026 pugur
// This source code is licensed under the Apache License, Version 2.0 with LLVM
// Exceptions which can be found in the LICENSE file.

#pragma once

#include <compare>

namespace base {

template <typename T, typename U>
struct Id {
  using IdType = U;
  IdType id;

  constexpr explicit Id(IdType id) : id(id) {}
  constexpr auto operator<=>(const Id&) const = default;

  // Preincrement
  constexpr Id& operator++() {
    ++id;
    return *this;
  }
};

template <typename T>
concept HasIdType = requires { typename T::IdType; };

}  // namespace base

