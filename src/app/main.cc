#include <iostream>

#include "base/add.h"

int main() {
  std::cout << "Hello World" << '\n'
            << "2 + 3 is " << add(2, 3) << '\n'
            << std::flush;
  return 0;
}
