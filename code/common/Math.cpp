#include "Math.h"
#include "Random.h"
#include <catch2/catch.hpp>

namespace common {

double rsqrt(double number) {
  float x2 = static_cast<float>(number) * 0.5F;
  float y = static_cast<float>(number);
  long i = *(long *)&y;
  i = 0x5f3759df - (i >> 1);
  y = *(float *)&i;
  y = y * (1.5F - (x2 * y * y));
  return static_cast<double>(y);
}

} // namespace common

TEST_CASE("math_rsqrt") {
  REQUIRE(common::rsqrt(4) == Approx(0.5f).epsilon(0.01));
}

static_assert(common::equal(1.0f, 1.0f));
static_assert(common::equal(-1.0f, -1.0f));
static_assert(common::equal(42.231f, 42.231f));
static_assert(!common::equal(1.0f, -1.0f));
static_assert(!common::equal(1.0001f, 1.0002f));
static_assert(!common::equal(1.0000f, 1.0001f));
static_assert(!common::equal(1.1f, 1.2f, 0.1));
static_assert(!common::equal(1.1f, 1.2f, 0.01));

TEST_CASE("equal") {
  REQUIRE(common::equal(1.0f, 1.0f));
  REQUIRE(common::equal(-1.0f, -1.0f));
  REQUIRE(common::equal(42.231f, 42.231f));
  REQUIRE(!common::equal(1.0f, -1.0f));
  REQUIRE(!common::equal(1.0001f, 1.0002f));
  REQUIRE(!common::equal(1.0000f, 1.0001f));
  REQUIRE(!common::equal(1.1f, 1.2f, 0.1));
  REQUIRE(!common::equal(1.1f, 1.2f, 0.01));
}
