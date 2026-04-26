// Copyright 2026 pugur
// This source code is licensed under the Apache License, Version 2.0 with LLVM
// Exceptions which can be found in the LICENSE file.

#include "app/init_handler.h"

#include "fpag/base/console.h"
#include "fpag/base/debug/signal_handler.h"
#include "fpag/base/debug/terminate_handler.h"
#include "fpag/base/exit_handler.h"

namespace app {

void init() {
  base::register_console();
  base::register_exit_handler();
  base::register_terminate_handler();
  base::register_signal_handlers();
}

}  // namespace app
