// Copyright 2026 pugur
// This source code is licensed under the Apache License, Version 2.0 with LLVM
// Exceptions which can be found in the LICENSE file.

#pragma once

#include <cmath>
#include <vector>

#include "base/arena.h"

namespace base {

template <typename T>
class ChunkedArenaArray {
 public:
  static constexpr usize kElementsPerChunk =
      (Arena::kChunkSize - sizeof(void*)) / sizeof(T);
  static_assert(kElementsPerChunk > 0);

  explicit ChunkedArenaArray(bool use_huage_pages = true)
      : use_huge_pages_(use_huage_pages) {}

  ~ChunkedArenaArray() = default;

  ChunkedArenaArray(const ChunkedArenaArray&) = delete;
  ChunkedArenaArray& operator=(const ChunkedArenaArray&) = delete;

  ChunkedArenaArray(ChunkedArenaArray&&) noexcept = default;
  ChunkedArenaArray& operator=(ChunkedArenaArray&&) noexcept = default;

  template <typename... Args>
  T* emplace_back(Args&&... args) {
    if (size_in_current_chunk_ >= kElementsPerChunk) {
      add_chunk();
    }

    T* storage = &chunks_.back()[size_in_current_chunk_];
    T* obj = new (storage) T(std::forward<Args>(args)...);

    size_in_current_chunk_++;
    total_size_++;
    return obj;
  }

  [[nodiscard]] inline T& operator[](usize index) {
    dcheck(index < total_size_);
    const usize chunk_idx = index / kElementsPerChunk;
    const usize offset = index % kElementsPerChunk;
    return chunks_[chunk_idx][offset];
  }

  [[nodiscard]] inline const T& operator[](usize index) const {
    dcheck(index < total_size_);
    const usize chunk_idx = index / kElementsPerChunk;
    const usize offset = index % kElementsPerChunk;
    return chunks_[chunk_idx][offset];
  }

  [[nodiscard]] inline usize size() const { return total_size_; }

 private:
  void add_chunk() {
    T* new_storage = static_cast<T*>(arena_.alloc(sizeof(T) * kElementsPerChunk,
                                                  use_huge_pages_, alignof(T)));

    chunks_.push_back(new_storage);
    size_in_current_chunk_ = 0;
  }

  std::vector<T*> chunks_;
  Arena arena_;
  usize total_size_ = 0;
  usize size_in_current_chunk_ = 0;
  bool use_huge_pages_ : 1;
};

}  // namespace base
