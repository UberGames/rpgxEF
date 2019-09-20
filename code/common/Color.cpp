#include "Color.h"

#include <catch2/catch.hpp>

static_assert(common::detail::color_check_bounds(-0.1f) == 0.0f);
static_assert(common::detail::color_check_bounds(255.1f) == 255.0f);
static_assert(common::detail::color_check_bounds(0.0f) == 0.0f);
static_assert(common::detail::color_check_bounds(255.0f) == 255.0f);
static_assert(common::detail::color_check_bounds(43.5f) == 43.5f);

static_assert(common::detail::color_check_bounds(-0.1) == 0.0f);
static_assert(common::detail::color_check_bounds(255.1) == 255.0f);
static_assert(common::detail::color_check_bounds(0.0) == 0.0f);
static_assert(common::detail::color_check_bounds(255.0) == 255.0f);
static_assert(common::detail::color_check_bounds(43.5) == 43.5f);

static_assert(common::detail::color_check_bounds(-1) == 0.0f);
static_assert(common::detail::color_check_bounds(256) == 255.0f);
static_assert(common::detail::color_check_bounds(0) == 0.0f);
static_assert(common::detail::color_check_bounds(255) == 255.0f);
static_assert(common::detail::color_check_bounds(43) == 43.0f);

static_assert(common::Color(1.0f).r_ == 1.0f);
static_assert(common::Color(1.0f).g_ == 0.0f);
static_assert(common::Color(1.0f).b_ == 0.0f);
static_assert(common::Color(1.0f).a_ == 0.0f);

static_assert(common::Color(1.0f, 2.4f).r_ == 1.0f);
static_assert(common::Color(1.0f, 2.4f).g_ == 2.4f);
static_assert(common::Color(1.0f, 2.4f).b_ == 0.0f);
static_assert(common::Color(1.0f, 2.4f).a_ == 0.0f);

static_assert(common::Color(1.0f, 2.4f, 3.5f).r_ == 1.0f);
static_assert(common::Color(1.0f, 2.4f, 3.5f).g_ == 2.4f);
static_assert(common::Color(1.0f, 2.4f, 3.5f).b_ == 3.5f);
static_assert(common::Color(1.0f, 2.4f, 3.5f).a_ == 0.0f);

static_assert(common::Color(1.0f, 2.4f, 3.5f, 54.43f).r_ == 1.0f);
static_assert(common::Color(1.0f, 2.4f, 3.5f, 54.43f).g_ == 2.4f);
static_assert(common::Color(1.0f, 2.4f, 3.5f, 54.43f).b_ == 3.5f);
static_assert(common::Color(1.0f, 2.4f, 3.5f, 54.43f).a_ == 54.43f);

TEST_CASE("default construct", "[common::Color]") {
  auto color = common::Color();
  REQUIRE_THAT(color.r_, Catch::WithinAbs(0.0f, 0.0001));
  REQUIRE_THAT(color.g_, Catch::WithinAbs(0.0f, 0.0001));
  REQUIRE_THAT(color.b_, Catch::WithinAbs(0.0f, 0.0001));
  REQUIRE_THAT(color.a_, Catch::WithinAbs(0.0f, 0.0001));

  REQUIRE_THAT(color.values[0], Catch::WithinAbs(0.0f, 0.0001));
  REQUIRE_THAT(color.values[1], Catch::WithinAbs(0.0f, 0.0001));
  REQUIRE_THAT(color.values[2], Catch::WithinAbs(0.0f, 0.0001));
  REQUIRE_THAT(color.values[3], Catch::WithinAbs(0.0f, 0.0001));
}

TEST_CASE("construct from numbers", "[common::Color]") {
  auto color = common::Color(42);
  REQUIRE_THAT(color.r_, Catch::WithinAbs(42.0f, 0.0001));
  REQUIRE_THAT(color.g_, Catch::WithinAbs(0.0f, 0.0001));
  REQUIRE_THAT(color.b_, Catch::WithinAbs(0.0f, 0.0001));
  REQUIRE_THAT(color.a_, Catch::WithinAbs(0.0f, 0.0001));

  color = common::Color(42, -42);
  REQUIRE_THAT(color.r_, Catch::WithinAbs(42.0f, 0.0001));
  REQUIRE_THAT(color.g_, Catch::WithinAbs(0.0f, 0.0001));
  REQUIRE_THAT(color.b_, Catch::WithinAbs(0.0f, 0.0001));
  REQUIRE_THAT(color.a_, Catch::WithinAbs(0.0f, 0.0001));

  color = common::Color(42, -42, 24, -22);
  REQUIRE_THAT(color.r_, Catch::WithinAbs(42.0f, 0.0001));
  REQUIRE_THAT(color.g_, Catch::WithinAbs(0.0f, 0.0001));
  REQUIRE_THAT(color.b_, Catch::WithinAbs(24.0f, 0.0001));
  REQUIRE_THAT(color.a_, Catch::WithinAbs(0.0f, 0.0001));

  color = common::Color(42.3, -42, 24, -22);
  REQUIRE_THAT(color.r_, Catch::WithinAbs(42.3f, 0.0001));
  REQUIRE_THAT(color.g_, Catch::WithinAbs(0.0f, 0.0001));
  REQUIRE_THAT(color.b_, Catch::WithinAbs(24.0f, 0.0001));
  REQUIRE_THAT(color.a_, Catch::WithinAbs(0.0f, 0.0001));

  color = common::Color(42.3, -42.2, 24, -22);
  REQUIRE_THAT(color.r_, Catch::WithinAbs(42.3f, 0.0001));
  REQUIRE_THAT(color.g_, Catch::WithinAbs(0.0f, 0.0001));
  REQUIRE_THAT(color.b_, Catch::WithinAbs(24.0f, 0.0001));
  REQUIRE_THAT(color.a_, Catch::WithinAbs(0.0f, 0.0001));

  color = common::Color(42.3, -42.2, 24, -22.7f);
  REQUIRE_THAT(color.r_, Catch::WithinAbs(42.3f, 0.0001));
  REQUIRE_THAT(color.g_, Catch::WithinAbs(0.0f, 0.0001));
  REQUIRE_THAT(color.b_, Catch::WithinAbs(24.0f, 0.0001));
  REQUIRE_THAT(color.a_, Catch::WithinAbs(0.0f, 0.0001));

  color = common::Color(42.3, 42.2, 24, 22.7f);
  REQUIRE_THAT(color.r_, Catch::WithinAbs(42.3f, 0.0001));
  REQUIRE_THAT(color.g_, Catch::WithinAbs(42.2f, 0.0001));
  REQUIRE_THAT(color.b_, Catch::WithinAbs(24.0f, 0.0001));
  REQUIRE_THAT(color.a_, Catch::WithinAbs(22.7f, 0.0001));
}
