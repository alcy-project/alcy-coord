// Copyright 2026 pugur
// This source code is licensed under the Apache License, Version 2.0 with LLVM
// Exceptions which can be found in the LICENSE file.

#pragma once

#include <cstddef>
#include <memory>
#include <type_traits>

#include "fpag/base/numeric.h"
#include "fpag/mem/page_allocator.h"

namespace base {

template <typename T>
struct SingleThreadPageAllocator {
  using value_type = T;

  constexpr SingleThreadPageAllocator() = default;
  constexpr ~SingleThreadPageAllocator() = default;

  template <typename U>
  constexpr explicit SingleThreadPageAllocator(
      const SingleThreadPageAllocator<U>&) noexcept {}

  static inline T* allocate(usize n) noexcept {
    return static_cast<T*>(mem::allocate_pages(n));
  }

  static inline void deallocate(T* p, std::size_t n) noexcept {
    mem::free_pages(p, n);
  }
};

template <typename T, typename U>
constexpr bool operator==(const SingleThreadPageAllocator<T>&,
                          const SingleThreadPageAllocator<U>&) {
  return true;
}

template <typename T, typename U>
constexpr bool operator!=(const SingleThreadPageAllocator<T>&,
                          const SingleThreadPageAllocator<U>&) {
  return false;
}

}  // namespace base
