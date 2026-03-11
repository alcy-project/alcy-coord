// Copyright 2026 pugur
// This source code is licensed under the Apache License, Version 2.0 with LLVM
// Exceptions which can be found in the LICENSE file.

#include "base/arena.h"

#include <algorithm>

#include "base/numeric.h"

namespace base {

Arena& Arena::operator=(Arena&& other) noexcept {
  if (this != &other) {
    reset();
    head_ = std::exchange(other.head_, nullptr);
  }
  return *this;
}

void* Arena::alloc(usize size, bool use_huge_pages, usize align) {
  // Check if the align is power of 2
  dcheck(align > 0 && (align & (align - 1)) == 0);

  if (head_) {
    if (void* const p = try_alloc_from_head(size, align)) {
      return p;
    }
  }

  const usize required_capacity = size + (align - 1);
  const usize alloc_capacity =
      std::max(kChunkSize - sizeof(Chunk), required_capacity);
  const usize total_alloc_size = sizeof(Chunk) + alloc_capacity;

  void* const raw_mem = use_huge_pages ? allocate_huge_pages(total_alloc_size)
                                       : allocate_pages(total_alloc_size);
  dcheck(raw_mem);

  Chunk* const new_chunk = new (raw_mem) Chunk();
  new_chunk->capacity = alloc_capacity;
  new_chunk->used = 0;
  new_chunk->next = head_;
  head_ = new_chunk;

  void* const p = try_alloc_from_head(size, align);
  dcheck(p);
  return p;
}

void Arena::reset() {
  Chunk* curr = head_;
  while (curr) {
    Chunk* const next = curr->next;
    free_pages(curr, sizeof(Chunk) + curr->capacity);
    curr = next;
  }
  head_ = nullptr;
}

void* Arena::try_alloc_from_head(usize size, usize align) {
  u8* const curr_ptr = head_->data() + head_->used;
  const uintptr_t curr_addr = reinterpret_cast<uintptr_t>(curr_ptr);

  const uintptr_t aligned_addr =
      (curr_addr + align - 1) & ~(uintptr_t(align) - 1);
  const usize padding = static_cast<usize>(aligned_addr - curr_addr);

  if (head_->used + padding + size <= head_->capacity) {
    head_->used += padding + size;
    // NOLINTNEXTLINE(performance-no-int-to-ptr)
    return reinterpret_cast<void*>(aligned_addr);
  }
  return nullptr;
}
}  // namespace base
