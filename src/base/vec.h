// Copyright 2026 pugur
// This source code is licensed under the Apache License, Version 2.0 with LLVM
// Exceptions which can be found in the LICENSE file.

#pragma once

#include <memory>
#include <type_traits>
#include <utility>
#include <vector>

#include "base/debug/check.h"
#include "base/id.h"
#include "fpag/base/numeric.h"

namespace base {

template <typename T,
          base::HasIdType Id,
          typename Allocator = std::allocator<T>>
class Vec {
 public:
  Vec() = default;
  ~Vec() = default;
  Vec(const Vec&) = delete;
  Vec& operator=(const Vec&) = delete;
  Vec(Vec&&) noexcept = default;
  Vec& operator=(Vec&&) noexcept = default;

  const T& operator[](const Id id) const {
    range_check(id);
    return vec_[id.id];
  }
  T& operator[](const Id id) {
    range_check(id);
    return vec_[id.id];
  }

  inline void reserve(usize size) { vec_.reserve(size); }
  inline void resize(usize size) { vec_.resize(size); }

  inline Id emplace_back(T&& obj) {
    vec_.emplace_back(std::move(obj));
    return Id(size() - 1);
  }

  inline Id emplace_back(T obj) {
    static_assert(std::is_trivially_copyable_v<T>);
    vec_.emplace_back(std::move(obj));
    return Id(static_cast<Id::IdType>((size() - 1)));
  }

  inline void pop_back() { vec_.pop_back(); }
  inline void shrink_to_fit() { vec_.shrink_to_fit(); }

  inline usize size() const { return vec_.size(); }
  inline usize capacity() const { return vec_.capacity(); }

  inline auto begin() const { return vec_.begin(); }
  inline auto end() const { return vec_.end(); }

  // inline const T* data() const { return vec_.data(); }

 private:
  inline void range_check(Id id) const {
    DCHECK_GE_MSG(id.id, 0, "Vec range check failed");
    DCHECK_LT_MSG(id.id, size(), "Vec range check failed");
  }

  std::vector<T, Allocator> vec_;
};

}  // namespace base
