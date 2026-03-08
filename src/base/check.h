// Copyright 2026 pugur
// This source code is licensed under the Apache License, Version 2.0 with LLVM
// Exceptions which can be found in the LICENSE file.

#pragma once

#include <cstdlib>
#include <print>

#include "base/log_prefix.h"

namespace base {

[[noreturn]] inline void dcheck_fail(const char* expr,
                                     const char* file,
                                     int line,
                                     const char* func) {
  std::print(stderr, "{}debug check failed: {}\n  at {}:{} ({})\n",
             fatal_prefix(), expr, file, line, func);
  std::abort();
}

}  // namespace base

#ifdef DEBUG
#define dcheck(expr)                                            \
  do {                                                          \
    if (__builtin_expect(!(expr), 0)) {                         \
      ::base::dcheck_fail(#expr, __FILE__, __LINE__, __func__); \
    }                                                           \
  } while (false)
#else
#define dcheck(expr) \
  do {               \
    (void)(expr);    \
  } while (false)
#endif
