#include "Vector.h"
#include "Random.h"
#include <catch2/catch.hpp>

namespace common {}

static_assert(common::Vector().x_ == 0.0f);
static_assert(common::Vector().y_ == 0.0f);
static_assert(common::Vector().z_ == 0.0f);

static_assert(common::Vector(1, 2, 3).x_ == 1.0f);
static_assert(common::Vector(1, 2, 3).y_ == 2.0f);
static_assert(common::Vector(1, 2, 3).z_ == 3.0f);

static_assert(common::Vector(1.0f, 2.5f, -3.123f).x_ == 1.0f);
static_assert(common::Vector(1.0f, 2.5f, -3.123f).y_ == 2.5f);
static_assert(common::Vector(1.0f, 2.5f, -3.123f).z_ == -3.123f);

static_assert(common::Vector(1.0, 2.5, -3.123).x_ == 1.0f);
static_assert(common::Vector(1.0, 2.5, -3.123).y_ == 2.5f);
static_assert(common::Vector(1.0, 2.5, -3.123).z_ == -3.123f);

TEST_CASE("vector_construct", "[common::Vector]") {
  REQUIRE(common::Vector().x_ == Approx(0.0f));
  REQUIRE(common::Vector().y_ == Approx(0.0f));
  REQUIRE(common::Vector().z_ == Approx(0.0f));

  REQUIRE(common::Vector(1, 2, 3).x_ == Approx(1.0f));
  REQUIRE(common::Vector(1, 2, 3).y_ == Approx(2.0f));
  REQUIRE(common::Vector(1, 2, 3).z_ == Approx(3.0f));

  REQUIRE(common::Vector(1.0f, 2.5f, -3.123f).x_ == Approx(1.0f));
  REQUIRE(common::Vector(1.0f, 2.5f, -3.123f).y_ == Approx(2.5f));
  REQUIRE(common::Vector(1.0f, 2.5f, -3.123f).z_ == Approx(-3.123f));

  REQUIRE(common::Vector(1.0, 2.5, -3.123).x_ == Approx(1.0f));
  REQUIRE(common::Vector(1.0, 2.5, -3.123).y_ == Approx(2.5f));
  REQUIRE(common::Vector(1.0, 2.5, -3.123).z_ == Approx(-3.123f));

  vec3_t vec_3{1, 2, 3};
  REQUIRE(common::Vector(vec_3).x_ == Approx(1.0f));
  REQUIRE(common::Vector(vec_3).y_ == Approx(2.0f));
  REQUIRE(common::Vector(vec_3).z_ == Approx(3.0f));
}

static_assert(common::dotProduct({1, 2, 3}, {3, 2, 1}) == 10.0f);
static_assert(common::dotProduct({1, 5, 3}, {3, 2, 1}) == 16.0f);

TEST_CASE("vector_dotproduct", "[common::Vector]") {
  REQUIRE(common::dotProduct({1, 2, 3}, {3, 2, 1}) == Approx(10.0f));
  REQUIRE(common::dotProduct({1, 5, 3}, {3, 2, 1}) == Approx(16.0f));

  for (auto i = 0; i < 100; i++) {
    auto v1 = common::Vector{common::random<float>(-100000, 100000),
                             common::random<float>(-100000, 100000),
                             common::random<float>(-100000, 100000)};
    auto v2 = common::Vector{common::random<float>(-100000, 100000),
                             common::random<float>(-100000, 100000),
                             common::random<float>(-100000, 100000)};

    REQUIRE(common::dotProduct(v1, v2) ==
            Approx(v1.x_ * v2.x_ + v1.y_ * v2.y_ + v1.z_ * v2.z_));
  }
}

static_assert((common::Vector{2, 5, 4} - common::Vector{1, -2, 4}).x_ == 1.0f);
static_assert((common::Vector{2, 5, 4} - common::Vector{1, -2, 4}).y_ == 7.0f);
static_assert((common::Vector{2, 5, 4} - common::Vector{1, -2, 4}).z_ == 0.0f);

TEST_CASE("vector_substract", "[common::Vector}") {
  REQUIRE((common::Vector{2, 5, 4} - common::Vector{1, -2, 4}).x_ == 1.0f);
  REQUIRE((common::Vector{2, 5, 4} - common::Vector{1, -2, 4}).y_ == 7.0f);
  REQUIRE((common::Vector{2, 5, 4} - common::Vector{1, -2, 4}).z_ == 0.0f);

  for (auto i = 0; i < 100; i++) {
    auto v1 = common::Vector{common::random<float>(-100000, 100000),
                             common::random<float>(-100000, 100000),
                             common::random<float>(-100000, 100000)};
    auto v2 = common::Vector{common::random<float>(-100000, 100000),
                             common::random<float>(-100000, 100000),
                             common::random<float>(-100000, 100000)};

    auto res = v1 - v2;
    REQUIRE(res.x_ == v1.x_ - v2.x_);
    REQUIRE(res.y_ == v1.y_ - v2.y_);
    REQUIRE(res.z_ == v1.z_ - v2.z_);
  }
}

static_assert((common::Vector{2, 5, 4} + common::Vector{1, -2, 4}).x_ == 3.0f);
static_assert((common::Vector{2, 5, 4} + common::Vector{1, -2, 4}).y_ == 3.0f);
static_assert((common::Vector{2, 5, 4} + common::Vector{1, -2, 4}).z_ == 8.0f);

TEST_CASE("vector_add", "[common::Vector]") {
  REQUIRE((common::Vector{2, 5, 4} + common::Vector{1, -2, 4}).x_ == 3.0f);
  REQUIRE((common::Vector{2, 5, 4} + common::Vector{1, -2, 4}).y_ == 3.0f);
  REQUIRE((common::Vector{2, 5, 4} + common::Vector{1, -2, 4}).z_ == 8.0f);

  for (auto i = 0; i < 100; i++) {
    auto v1 = common::Vector{common::random<float>(-100000, 100000),
                             common::random<float>(-100000, 100000),
                             common::random<float>(-100000, 100000)};
    auto v2 = common::Vector{common::random<float>(-100000, 100000),
                             common::random<float>(-100000, 100000),
                             common::random<float>(-100000, 100000)};

    auto res = v1 + v2;
    REQUIRE(res.x_ == v1.x_ + v2.x_);
    REQUIRE(res.y_ == v1.y_ + v2.y_);
    REQUIRE(res.z_ == v1.z_ + v2.z_);
  }
}
