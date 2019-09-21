#include "Random.h"
#include <catch2/catch.hpp>

TEST_CASE("random_int", "[common::getRandom]") {
  for (auto x = 0; x < 1000; x++) {
    auto i = common::getRandom<int>();
    auto a = common::getRandom<int>();
    REQUIRE(i != a);
  }
}

TEST_CASE("random_int_limit", "[common::getRandom]") {
  for (auto j = 0; j < 10; j++) {
    auto min = common::getRandom<int>();
    auto max = common::getRandom<int>();
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
      auto a = common::getRandom(min, max);
      REQUIRE(a >= min);
      REQUIRE(a <= max);
    }
  }
}

TEST_CASE("random_real", "[common::getRandom]") {
  for (auto x = 0; x < 1000; x++) {
    auto a = common::getRandom<double>();
    auto b = common::getRandom<double>();
    REQUIRE(a != b);
  }
}

TEST_CASE("random_real_limit", "[common::getRandom]") {
  for (auto j = 0; j < 10; j++) {
    auto min = common::getRandom<double>();
    auto max = common::getRandom<double>();
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
      auto a = common::getRandom(min, max);
      REQUIRE(a >= min);
      REQUIRE(a <= max);
    }
  }
}
