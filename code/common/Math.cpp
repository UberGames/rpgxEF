#include "Math.h"
#include "Random.h"
#include <catch2/catch.hpp>

namespace common {

float rsqrt(float number) {
  float x2 = number * 0.5F;
  float y = number;
  long i = *(long *)&y;
  i = 0x5f3759df - (i >> 1);
  y = *(float *)&i;
  y = y * (1.5F - (x2 * y * y));
  return y;
}

} // namespace common

TEST_CASE("math_rsqrt") {
  REQUIRE(common::rsqrt(4) == Approx(0.5f).epsilon(0.01));
}
