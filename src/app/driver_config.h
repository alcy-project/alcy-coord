// Copyright 2026 pugur
// This source code is licensed under the Apache License, Version 2.0 with LLVM
// Exceptions which can be found in the LICENSE file.

#pragma once

#include "fpag/base/color_mode.h"
#include "fpag/base/numeric.h"

namespace app {

struct DriverConfig {
  bool time_trace;
  base::ColorMode color_mode;
};

}  // namespace app
