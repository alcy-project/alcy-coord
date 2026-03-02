#define CATCH_CONFIG_MAIN
#include "base/add.h"

#include <catch2/catch_all.hpp>

TEST_CASE("simple addition") {
  REQUIRE(add(1, 1) == 2);
}
