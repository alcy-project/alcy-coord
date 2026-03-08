// Copyright 2026 pugur
// This source code is licensed under the Apache License, Version 2.0 with LLVM
// Exceptions which can be found in the LICENSE file.

#include <iostream>

#include "base/add.h"

int main() {
  std::cout << "Hello World" << '\n'
            << "2 + 3 is " << add(2, 3) << '\n'
            << std::flush;
  return 0;
}
