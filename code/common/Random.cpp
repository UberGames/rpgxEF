#include "Random.h"
#include <catch2/catch.hpp>

TEST_CASE("random_int", "[common::random]") {
  for (auto x = 0; x < 1000; x++) {
    auto i = common::random<int>();
    auto a = common::random<int>();
    REQUIRE(i != a);
  }
}

TEST_CASE("random_int_limit", "[common::random]") {
  for (auto j = 0; j < 10; j++) {
    auto min = common::random<int>();
    auto max = common::random<int>();
    if (min > max) {
      std::swap(min, max);
    }
    if (min == max) {
      if (min == std::numeric_limits<int>::min()) {
        max++;
      } else if (min == std::numeric_limits<int>::max()) {
        min--;
      }
    }

    for (auto x = 0; x < 100; x++) {
      auto a = common::random(min, max);
      REQUIRE(a >= min);
      REQUIRE(a <= max);
    }
  }
}

TEST_CASE("random_real", "[common::random]") {
  for (auto x = 0; x < 1000; x++) {
    auto a = common::random<double>();
    auto b = common::random<double>();
    REQUIRE(a != b);
  }
}

TEST_CASE("random_real_limit", "[common::random]") {
  for (auto j = 0; j < 10; j++) {
    auto min = common::random<double>();
    auto max = common::random<double>();
    if (min > max) {
      std::swap(min, max);
    }
    if (min == max) {
      if (min == std::numeric_limits<double>::min()) {
        max++;
      } else if (min == std::numeric_limits<double>::max()) {
        min--;
      }
    }

    for (auto x = 0; x < 100; x++) {
      auto a = common::random(min, max);
      REQUIRE(a >= min);
      REQUIRE(a <= max);
    }
  }
}
