#include "Random.h"
#include <catch2/catch.hpp>

namespace common {

int getRandomInt() { return getRandomInt<int>(); }

double getRandomDouble() { return getRandomDouble<double>(); }

} // namespace common

TEST_CASE("random_int", "[common::getRandom]") {
  for (auto x = 0; x < 1000; x++) {
    auto i = common::getRandomInt<int>();
    auto a = common::getRandomInt<int>();
    REQUIRE(i != a);
  }
}

TEST_CASE("random_int_limit", "[common::getRandom]") {
  for (auto j = 0; j < 10; j++) {
    auto min = common::getRandomInt<int>();
    auto max = common::getRandomInt<int>();
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
      auto a = common::getRandomInt(min, max);
      REQUIRE(a >= min);
      REQUIRE(a <= max);
    }
  }
}

TEST_CASE("random_real", "[common::getRandom]") {
  for (auto x = 0; x < 1000; x++) {
    auto a = common::getRandomDouble<double>();
    auto b = common::getRandomDouble<double>();
    REQUIRE(a != b);
  }
}

TEST_CASE("random_real_limit", "[common::getRandom]") {
  for (auto j = 0; j < 10; j++) {
    auto min = common::getRandomDouble<double>();
    auto max = common::getRandomDouble<double>();
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
      auto a = common::getRandomDouble(min, max);
      REQUIRE(a >= min);
      REQUIRE(a <= max);
    }
  }
}
