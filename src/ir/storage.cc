// Copyright 2026 pugur
// This source code is licensed under the Apache License, Version 2.0 with LLVM
// Exceptions which can be found in the LICENSE file.

#include "ir/storage.h"

#include <string>
#include <type_traits>

#include "fpag/str/format_util.h"
#include "ir/block.h"
#include "ir/instruction.h"

namespace ir {

std::string Storage::dump() const {
  std::string temp = "";
  temp.append(str::format("[meta]\nnum_blocks = {}\nnum_instructions = {}\n",
                          num_blocks(), num_instructions()));
  for (usize i = 0; i < num_blocks(); ++i) {
    const Block b = block(i);
    temp.append(str::format("\n[[blocks]]\nindex = {}\nhead = {}\nlen = {}\n\n",
                            i, b.head, b.length));
    for (usize j = b.head; j < b.end(); ++j) {
      const Instruction inst = instruction(j);
      temp.append(str::format(
          "[[blocks.instructions]]\nindex = {}\nop = {}\ndst = "
          "{}\nlhs.tag = {}\nrhs.tag = {}\n",
          j, static_cast<u8>(inst.op), inst.dst.id,
          static_cast<u8>(inst.lhs.tag), static_cast<u8>(inst.rhs.tag)));
    }
  }
  return temp;
}

}  // namespace ir
