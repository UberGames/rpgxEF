#include "Color.h"

namespace common {

const Color Color::None = Color{0.0f, 0.0f, 0.0f, 0.0f};
const Color Color::Black = Color{0, 0, 0, 1};
const Color Color::Red = Color{1, 0, 0, 1};
const Color Color::Green = Color{0, 1, 0, 1};
const Color Color::Blue = Color{0, 0, 1, 1};
const Color Color::Yellow = Color{1, 1, 0, 1};
const Color Color::Magenta = Color{1, 0, 1, 1};
const Color Color::Cyan = Color{0, 1, 1, 1};
const Color Color::Teal = Color{0.071f, 0.271f, 0.29f, 1};
const Color Color::Gold = Color{0.529f, 0.373f, 0.017f, 1};
const Color Color::White = Color{1, 1, 1, 1};
const Color Color::LightGrey = Color{0.75f, 0.75f, 0.75f, 1};
const Color Color::MediumGrey = Color{0.50f, 0.50f, 0.50f, 1};
const Color Color::DarkGrey = Color{0.25f, 0.25f, 0.25f, 1};
const Color Color::DarkGrey2 = Color{0.15f, 0.15f, 0.15f, 1};

const Color Color::VeryLightOrange = Color{0.688f, 0.797f, 1, 1};
const Color Color::LightOrange = Color{0.688f, 0.797f, 1, 1};
const Color Color::DarkOrange = Color{0.620f, 0.710f, 0.894f, 1};
const Color Color::VeryDarkOrange = Color{0.463f, 0.525f, 0.671f, 1};

const Color Color::VeryLightBlue = Color{0.616f, 0.718f, 0.898f, 1};
const Color Color::LightBlue = Color{0.286f, 0.506f, 0.898f, 1};
const Color Color::DarkBlue = Color{0.082f, 0.388f, 0.898f, 1};
const Color Color::VeryDarkBlue = Color{0.063f, 0.278f, 0.514f, 1};

const Color Color::VeryLightBlue2 = Color{0.302f, 0.380f, 0.612f, 1};
const Color Color::LightBlue2 = Color{0.196f, 0.314f, 0.612f, 1};
const Color Color::DarkBlue2 = Color{0.060f, 0.227f, 0.611f, 1};
const Color Color::VeryDarkBlue2 = Color{0.043f, 0.161f, 0.459f, 1};

const Color Color::VeryLightBrown = Color{0.082f, 0.388f, 0.898f, 1};
const Color Color::LightBrown = Color{0.082f, 0.388f, 0.898f, 1};
const Color Color::DarkBrown = Color{0.078f, 0.320f, 0.813f, 1};
const Color Color::VeryDarkBrown = Color{0.060f, 0.227f, 0.611f, 1};

const Color Color::VeryLightGold = Color{1, 0.784f, 0.365f, 1};
const Color Color::LightGold = Color{1, 0.706f, 0.153f, 1};
const Color Color::DarkGold = Color{0.733f, 0.514f, 0.086f, 1};
const Color Color::VeryDarkGold = Color{0.549f, 0.384f, 0.063f, 1};

const Color Color::VeryLightPurple = Color{0.688f, 0.797f, 1, 1};
const Color Color::LightPurple = Color{0.688f, 0.797f, 1, 1};
const Color Color::DarkPurple = Color{0.313f, 0.578f, 1, 1};
const Color Color::VeryDarkPurple = Color{0.031f, 0.110f, 0.341f, 1};

const Color Color::VeryLightPurple2 = Color{0.688f, 0.797f, 1, 1};
const Color Color::LightPurple2 = Color{0.688f, 0.797f, 1, 1};
const Color Color::DarkPurple2 = Color{0.688f, 0.797f, 1, 1};
const Color Color::VeryDarkPurple2 = Color{0.031f, 0.110f, 0.341f, 1};

const Color Color::VeryLightPurple3 = Color{0.686f, 0.808f, 0.1f, 1};
const Color Color::LightPurple3 = Color{0.188f, 0.494f, 1, 1};
const Color Color::DarkPurple3 = Color{0.094f, 0.471f, 1, 1};
const Color Color::VeryDarkPurple3 = Color{0.067f, 0.325f, 0.749f, 1};

const Color Color::VeryLightRed = Color{1, 0.612f, 0.325f, 1};
const Color Color::LightRed = Color{1, 0.478f, 0.098f, 1};
const Color Color::DarkRed = Color{1, 0.438f, 0, 1};
const Color Color::VeryDarkRed = Color{0.784f, 0.329f, 0, 1};

} // namespace common

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

static_assert(common::Color(1.0f).r == 1.0f);
static_assert(common::Color(1.0f).g == 0.0f);
static_assert(common::Color(1.0f).b == 0.0f);
static_assert(common::Color(1.0f).a == 0.0f);

static_assert(common::Color(1.0f, 2.4f).r == 1.0f);
static_assert(common::Color(1.0f, 2.4f).g == 2.4f);
static_assert(common::Color(1.0f, 2.4f).b == 0.0f);
static_assert(common::Color(1.0f, 2.4f).a == 0.0f);

static_assert(common::Color(1.0f, 2.4f, 3.5f).r == 1.0f);
static_assert(common::Color(1.0f, 2.4f, 3.5f).g == 2.4f);
static_assert(common::Color(1.0f, 2.4f, 3.5f).b == 3.5f);
static_assert(common::Color(1.0f, 2.4f, 3.5f).a == 0.0f);

static_assert(common::Color(1.0f, 2.4f, 3.5f, 54.43f).r == 1.0f);
static_assert(common::Color(1.0f, 2.4f, 3.5f, 54.43f).g == 2.4f);
static_assert(common::Color(1.0f, 2.4f, 3.5f, 54.43f).b == 3.5f);
static_assert(common::Color(1.0f, 2.4f, 3.5f, 54.43f).a == 54.43f);

TEST_CASE("default construct", "[common::Color]") {
  auto color = common::Color();
  REQUIRE_THAT(color.r, Catch::WithinAbs(0.0f, 0.0001));
  REQUIRE_THAT(color.g, Catch::WithinAbs(0.0f, 0.0001));
  REQUIRE_THAT(color.b, Catch::WithinAbs(0.0f, 0.0001));
  REQUIRE_THAT(color.a, Catch::WithinAbs(0.0f, 0.0001));

  REQUIRE_THAT(color.values[0], Catch::WithinAbs(0.0f, 0.0001));
  REQUIRE_THAT(color.values[1], Catch::WithinAbs(0.0f, 0.0001));
  REQUIRE_THAT(color.values[2], Catch::WithinAbs(0.0f, 0.0001));
  REQUIRE_THAT(color.values[3], Catch::WithinAbs(0.0f, 0.0001));
}

TEST_CASE("construct from numbers", "[common::Color]") {
  auto color = common::Color(42);
  REQUIRE_THAT(color.r, Catch::WithinAbs(42.0f, 0.0001));
  REQUIRE_THAT(color.g, Catch::WithinAbs(0.0f, 0.0001));
  REQUIRE_THAT(color.b, Catch::WithinAbs(0.0f, 0.0001));
  REQUIRE_THAT(color.a, Catch::WithinAbs(0.0f, 0.0001));

  color = common::Color(42, -42);
  REQUIRE_THAT(color.r, Catch::WithinAbs(42.0f, 0.0001));
  REQUIRE_THAT(color.g, Catch::WithinAbs(0.0f, 0.0001));
  REQUIRE_THAT(color.b, Catch::WithinAbs(0.0f, 0.0001));
  REQUIRE_THAT(color.a, Catch::WithinAbs(0.0f, 0.0001));

  color = common::Color(42, -42, 24, -22);
  REQUIRE_THAT(color.r, Catch::WithinAbs(42.0f, 0.0001));
  REQUIRE_THAT(color.g, Catch::WithinAbs(0.0f, 0.0001));
  REQUIRE_THAT(color.b, Catch::WithinAbs(24.0f, 0.0001));
  REQUIRE_THAT(color.a, Catch::WithinAbs(0.0f, 0.0001));

  color = common::Color(42.3, -42, 24, -22);
  REQUIRE_THAT(color.r, Catch::WithinAbs(42.3f, 0.0001));
  REQUIRE_THAT(color.g, Catch::WithinAbs(0.0f, 0.0001));
  REQUIRE_THAT(color.b, Catch::WithinAbs(24.0f, 0.0001));
  REQUIRE_THAT(color.a, Catch::WithinAbs(0.0f, 0.0001));

  color = common::Color(42.3, -42.2, 24, -22);
  REQUIRE_THAT(color.r, Catch::WithinAbs(42.3f, 0.0001));
  REQUIRE_THAT(color.g, Catch::WithinAbs(0.0f, 0.0001));
  REQUIRE_THAT(color.b, Catch::WithinAbs(24.0f, 0.0001));
  REQUIRE_THAT(color.a, Catch::WithinAbs(0.0f, 0.0001));

  color = common::Color(42.3, -42.2, 24, -22.7f);
  REQUIRE_THAT(color.r, Catch::WithinAbs(42.3f, 0.0001));
  REQUIRE_THAT(color.g, Catch::WithinAbs(0.0f, 0.0001));
  REQUIRE_THAT(color.b, Catch::WithinAbs(24.0f, 0.0001));
  REQUIRE_THAT(color.a, Catch::WithinAbs(0.0f, 0.0001));

  color = common::Color(42.3, 42.2, 24, 22.7f);
  REQUIRE_THAT(color.r, Catch::WithinAbs(42.3f, 0.0001));
  REQUIRE_THAT(color.g, Catch::WithinAbs(42.2f, 0.0001));
  REQUIRE_THAT(color.b, Catch::WithinAbs(24.0f, 0.0001));
  REQUIRE_THAT(color.a, Catch::WithinAbs(22.7f, 0.0001));
}
