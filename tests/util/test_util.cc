// Copyright 2026 pugur
// This source code is licensed under the Apache License, Version 2.0 with LLVM
// Exceptions which can be found in the LICENSE file.

#include "util/test_util.h"

#include <memory>

#include "fpag/base/console.h"
#include "fpag/logging/sink/stdout_sink.h"
#include "fpag/logging/sync/sync_logger.h"
#include "fpag/mem/page_allocator.h"

namespace tests {

TestLogger& test_logger() {
  static TestLogger logger = [] {
    TestLogger l;
    l.init(logging::StdoutSink(
        static_cast<char*>(mem::allocate_pages(mem::kPageSize)), mem::kPageSize,
        base::console_color_style(base::Stream::Stdout), true));
    return l;
  }();
  return logger;
}

}  // namespace tests
