// Copyright 2026 pugur
// This source code is licensed under the Apache License, Version 2.0 with LLVM
// Exceptions which can be found in the LICENSE file.

#pragma once

#ifdef DEBUG
#include <cstdlib>
#include <print>
#include <utility>

#include "base/log_prefix.h"
#include "base/numeric.h"
#endif

namespace base {

#ifdef DEBUG
template <typename... Args>
inline void dlog_internal(const char* file,
                          i32 line,
                          const char* func,
                          std::format_string<Args...> fmt,
                          Args&&... args) {
  std::print("{}{} on {} ({}:{})\n", debug_prefix(),
             std::format(fmt, std::forward<Args>(args)...), func, file, line);
}

[[noreturn]] inline void dcheck_fail(const char* expr,
                                     const char* file,
                                     i32 line,
                                     const char* func) {
  std::print(stderr, "{}debug check failed: {}\n  at {}:{} ({})\n",
             fatal_prefix(), expr, file, line, func);
  std::abort();
}
#endif

}  // namespace base

#define noop(...)              \
  do {                         \
    (void)sizeof(__VA_ARGS__); \
  } while (false)

#ifdef DEBUG
#define on_debug(expr) expr
#define on_debug_raw(expr) on_debug(expr)
#define dlog(fmt, ...)                                \
  ::base::dlog_internal(__FILE__, __LINE__, __func__, \
                        fmt __VA_OPT__(, ) __VA_ARGS__)  // NOLINT
#define dcheck(expr)                                            \
  do {                                                          \
    if (!static_cast<bool>(expr)) [[unlikely]] {                \
      ::base::dcheck_fail(#expr, __FILE__, __LINE__, __func__); \
    }                                                           \
  } while (false)
#define dcheck_raw(expr) dcheck(expr)
#else
#define on_debug(expr) noop(expr)
#define on_debug_raw(expr)
#define dlog(fmt, ...) noop(fmt __VA_OPT__(, ) __VA_ARGS__)  // NOLINT
#define dcheck(expr) noop(expr)
#define dcheck_raw(expr)
#endif
