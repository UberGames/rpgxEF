#include "BitTools.h"
#include <catch2/catch.hpp>

namespace common {

static_assert(!bit_in(0, 1));
static_assert(!bit_in(0, 2));
static_assert(bit_in(1,1));
static_assert(bit_in(3, 1));
static_assert(bit_in(7, 4));
static_assert(!bit_in(5, 2));

TEST_CASE("bit_in") {
  REQUIRE(!bit_in(0, 1));
  REQUIRE(!bit_in(0, 2));
  REQUIRE(bit_in(1,1));
  REQUIRE(bit_in(3, 1));
  REQUIRE(bit_in(7, 4));
  REQUIRE(!bit_in(5, 2));
}

}
