#pragma once

#include "base/int.h"

namespace ir {

enum class OpCode : u8 {
  Add = 0,
  Sub = 1,
  Mul = 2,
  Div = 3,
  Mod = 4,
};

}  // namespace ir
