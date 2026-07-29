// Copyright 2026 pugur
// This source code is licensed under the Apache License, Version 2.0 with LLVM
// Exceptions which can be found in the LICENSE file.

#include "catch2/catch_session.hpp"
#include "fpag/base/numeric.h"
#include "fpag/debug/signal_handler.h"
#include "fpag/debug/terminate_handler.h"
#include "fpag/term/console.h"
#include "util/test_util.h"

#define CATCH_CONFIG_RUNNER

void init() {
  term::register_console();
  debug::register_terminate_handler();
  debug::register_signal_handlers();
  tests::init_logger();
}

void clean_up() {
  // noop
}

i32 main(i32 argc, char** argv) {
  init();

  Catch::Session session;

  const i32 return_code = session.applyCommandLine(argc, argv);
  if (return_code != 0) {
    return return_code;
  }

  const i32 result = session.run();

  clean_up();

  return result;
}

