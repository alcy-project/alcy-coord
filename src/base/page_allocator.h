// Copyright 2026 pugur
// This source code is licensed under the Apache License, Version 2.0 with LLVM
// Exceptions which can be found in the LICENSE file.

#pragma once

#include "base/numeric.h"

namespace base {

void* reserve_pages(usize size);
bool commit_pages(void* ptr, usize size);
void decommit_pages(void* ptr, usize size);

void* allocate_pages(usize size);
void* allocate_huge_pages(usize size);
void free_pages(void* ptr, usize size);

}  // namespace base
