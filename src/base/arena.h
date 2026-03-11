// Copyright 2026 pugur
// This source code is licensed under the Apache License, Version 2.0 with LLVM
// Exceptions which can be found in the LICENSE file.

#pragma once

#include <memory>
#include <utility>

#include "base/arena_deleter.h"
#include "base/debug.h"
#include "base/numeric.h"
#include "base/page_allocator.h"

namespace base {

class Arena {
 public:
  Arena() = default;
  ~Arena() { reset(); }

  Arena(const Arena&) = delete;
  Arena& operator=(const Arena&) = delete;

  Arena(Arena&& other) noexcept : head_(std::exchange(other.head_, nullptr)) {}
  Arena& operator=(Arena&& other) noexcept;

  [[nodiscard]] void* alloc(usize size,
                            bool use_huge_pages,
                            usize align = alignof(std::max_align_t));
  void reset();

  // Doesn't call destructor
  // Use for trivial copyable types
  template <typename T, typename... Args>
  [[nodiscard]] inline T* create(Args&&... args) {
    void* mem = alloc(sizeof(T), alignof(T));
    T* const obj = new (mem) T(std::forward<Args>(args)...);
    dcheck(obj);
    return obj;
  }

  // Calls destructor automatically
  template <typename T, typename... Args>
  [[nodiscard]] inline ArenaUniquePtr<T> create_managed(Args&&... args) {
    return ArenaUniquePtr<T>(create<T>(std::forward<Args>(args)...));
  }

  // 2 MiB
  static constexpr usize kChunkSize = 2 * 1024 * 1024;

 private:
  void* try_alloc_from_head(usize size, usize align);

  struct alignas(std::max_align_t) Chunk {
    Chunk* next = nullptr;
    usize capacity = 0;
    usize used = 0;

    // Can be written after the header
    u8* data() { return reinterpret_cast<u8*>(this + 1); }
  };

  Chunk* head_ = nullptr;
};

}  // namespace base
