// Copyright 2026 pugur
// This source code is licensed under the Apache License, Version 2.0 with LLVM
// Exceptions which can be found in the LICENSE file.

#pragma once

namespace ir {

struct InstuctionFlags {
  bool is_var_len : 1 = false;  // For call, etc.
};

}  // namespace ir
