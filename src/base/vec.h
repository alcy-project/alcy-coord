// Copyright 2026 pugur
// This source code is licensed under the Apache License, Version 2.0 with LLVM
// Exceptions which can be found in the LICENSE file.

#pragma once

#include <memory>
#include <type_traits>
#include <utility>
#include <vector>

#include "base/idx.h"
#include "base/idx_range.h"
#include "base/vec_slice.h"

namespace base {

template <typename T,
          base::HasIdxType Idx,
          typename Allocator = std::allocator<T>>
class Vec {
 public:
  using IdxType = typename Idx::IdxType;

  Vec() = default;
  ~Vec() = default;
  Vec(const Vec&) = delete;
  Vec& operator=(const Vec&) = delete;
  Vec(Vec&&) noexcept = default;
  Vec& operator=(Vec&&) noexcept = default;

  const T& operator[](const Idx idx) const { return vec_[idx.idx]; }
  T& operator[](const Idx idx) { return vec_[idx.idx]; }

  inline void reserve(usize size) { vec_.reserve(size); }
  inline void resize(usize size) { vec_.resize(size); }
  inline void resize(usize size, T init_value) {
    vec_.resize(size, init_value);
  }

  template <typename... Args>
  inline Idx emplace_back(Args&&... args) {
    vec_.emplace_back(std::forward<Args>(args)...);
    return Idx(static_cast<IdxType>(size() - 1));
  }

  inline void pop_back() { vec_.pop_back(); }
  inline void shrink_to_fit() { vec_.shrink_to_fit(); }

  inline usize size() const { return vec_.size(); }
  inline usize capacity() const { return vec_.capacity(); }

  inline auto begin() const { return vec_.begin(); }
  inline auto end() const { return vec_.end(); }

  inline IdxRange<Idx> idx_range() const {
    return IdxRange<Idx>(Idx{0}, static_cast<IdxType>(vec_.size()));
  }

  inline const T* data() const { return vec_.data(); }
  inline T* data() { return vec_.data(); }

  inline VecSlice<T, Idx> slice(IdxType offset, IdxType size) {
    return VecSlice<T, Idx>(data() + offset, size);
  }
  inline VecSlice<T, Idx> slice() { return slice(0, size()); }

  inline VecSlice<const T, Idx> slice(IdxType offset, IdxType size) const {
    return VecSlice<T, Idx>(data() + offset, size);
  }
  inline VecSlice<const T, Idx> slice() const { return slice(0, size()); }

 private:
  std::vector<T, Allocator> vec_;
};

}  // namespace base
