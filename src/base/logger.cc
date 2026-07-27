// Copyright 2026 pugur
// This source code is licensed under the Apache License, Version 2.0 with LLVM
// Exceptions which can be found in the LICENSE file.

#include "base/logger.h"

#include <utility>

#include "fpag/base/debug/logger.h"
// #include "fpag/mem/page_allocator.h"

namespace base {

Logger logger;

void init_logger(ColorStyle style) {
  logging::StdoutSink sink(nullptr, 0, style, false);
  logger.init(std::move(sink));
  init_debug_logger();
}

}  // namespace base
