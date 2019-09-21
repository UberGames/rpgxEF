#include "Vector.h"
#include "Random.h"
#include <catch2/catch.hpp>

namespace common {

constexpr Vector &Vector::clear() {
  x_ = 0;
  y_ = 0;
  z_ = 0;
  return *this;
}

float Vector::length() const { return sqrt(dotProduct(*this, *this)); }

constexpr float Vector::length_squared() const {
  return dotProduct(*this, *this);
}

} // namespace common

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
    auto v1 = common::Vector{common::getRandom<float>(-100000, 100000),
                             common::getRandom<float>(-100000, 100000),
                             common::getRandom<float>(-100000, 100000)};
    auto v2 = common::Vector{common::getRandom<float>(-100000, 100000),
                             common::getRandom<float>(-100000, 100000),
                             common::getRandom<float>(-100000, 100000)};

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
    auto v1 = common::Vector{common::getRandom<float>(-100000, 100000),
                             common::getRandom<float>(-100000, 100000),
                             common::getRandom<float>(-100000, 100000)};
    auto v2 = common::Vector{common::getRandom<float>(-100000, 100000),
                             common::getRandom<float>(-100000, 100000),
                             common::getRandom<float>(-100000, 100000)};

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
    auto v1 = common::Vector{common::getRandom<float>(-100000, 100000),
                             common::getRandom<float>(-100000, 100000),
                             common::getRandom<float>(-100000, 100000)};
    auto v2 = common::Vector{common::getRandom<float>(-100000, 100000),
                             common::getRandom<float>(-100000, 100000),
                             common::getRandom<float>(-100000, 100000)};

    auto res = v1 + v2;
    REQUIRE(res.x_ == v1.x_ + v2.x_);
    REQUIRE(res.y_ == v1.y_ + v2.y_);
    REQUIRE(res.z_ == v1.z_ + v2.z_);
  }
}

static_assert(common::Vector{1, 2, 3}.clear().x_ == 0.0f);
static_assert(common::Vector{1, 2, 3}.clear().y_ == 0.0f);
static_assert(common::Vector{1, 2, 3}.clear().z_ == 0.0f);

TEST_CASE("vector_clear", "[common::Vector]") {
  REQUIRE(common::Vector{1, 2, 3}.clear().x_ == 0.0f);
  REQUIRE(common::Vector{1, 2, 3}.clear().y_ == 0.0f);
  REQUIRE(common::Vector{1, 2, 3}.clear().z_ == 0.0f);
}

static_assert(common::ma(common::Vector{1, 2, 3}, 2, common::Vector{4, 5, 6})
                  .x_ == 9.00f);
static_assert(common::ma(common::Vector{1, 2, 3}, 2, common::Vector{4, 5, 6})
                  .y_ == 12.0f);
static_assert(common::ma(common::Vector{1, 2, 3}, 2, common::Vector{4, 5, 6})
                  .z_ == 15.0f);

TEST_CASE("vector_ma", "[common::Vector]") {
  REQUIRE(common::ma(common::Vector{1, 2, 3}, 2, common::Vector{4, 5, 6}).x_ ==
          9.00f);
  REQUIRE(common::ma(common::Vector{1, 2, 3}, 2, common::Vector{4, 5, 6}).y_ ==
          12.0f);
  REQUIRE(common::ma(common::Vector{1, 2, 3}, 2, common::Vector{4, 5, 6}).z_ ==
          15.0f);
}

static_assert((-common::Vector{1, 2, -5}).x_ == -1.0f);
static_assert((-common::Vector{1, 2, -5}).y_ == -2.0f);
static_assert((-common::Vector{1, 2, -5}).z_ == 5.0f);

static_assert((-common::Vector{-1, -2, 5}).x_ == 1.0f);
static_assert((-common::Vector{-1, -2, 5}).y_ == 2.0f);
static_assert((-common::Vector{-1, -2, 5}).z_ == -5.0f);

TEST_CASE("vector_negate", "[common::Vector]") {
  REQUIRE((-common::Vector{1, 2, -5}).x_ == -1.0f);
  REQUIRE((-common::Vector{1, 2, -5}).y_ == -2.0f);
  REQUIRE((-common::Vector{1, 2, -5}).z_ == 5.0f);

  REQUIRE((-common::Vector{-1, -2, 5}).x_ == 1.0f);
  REQUIRE((-common::Vector{-1, -2, 5}).y_ == 2.0f);
  REQUIRE((-common::Vector{-1, -2, 5}).z_ == -5.0f);
}

TEST_CASE("vector_length", "[common::Vector]") {
  REQUIRE(common::Vector{0, 0, 0}.length() == Approx(0.0f));
  REQUIRE(common::Vector{1, 0, 0}.length() == Approx(1.0f));
  REQUIRE(common::Vector{0, 1, 0}.length() == Approx(1.0f));
  REQUIRE(common::Vector{0, 0, 1}.length() == Approx(1.0f));
  REQUIRE(common::Vector{1, 1, 1}.length() == Approx(1.7320508075f));
  REQUIRE(common::Vector{22, 54, -1}.length() == Approx(58.3180932472f));
}

static_assert(common::Vector{0, 0, 0}.length_squared() == 0.0f);
static_assert(common::Vector{1, 0, 0}.length_squared() == 1.0f);
static_assert(common::Vector{0, 1, 0}.length_squared() == 1.0f);
static_assert(common::Vector{0, 0, 1}.length_squared() == 1.0f);
static_assert(common::Vector{1, 1, 1}.length_squared() == 3.0f);
static_assert(common::Vector{22, 54, -1}.length_squared() == 3401.0f);

TEST_CASE("vector_length_squared", "[common::Vector]") {
  REQUIRE(common::Vector{0, 0, 0}.length_squared() == 0.0f);
  REQUIRE(common::Vector{1, 0, 0}.length_squared() == 1.0f);
  REQUIRE(common::Vector{0, 1, 0}.length_squared() == 1.0f);
  REQUIRE(common::Vector{0, 0, 1}.length_squared() == 1.0f);
  REQUIRE(common::Vector{1, 1, 1}.length_squared() == 3.0f);
  REQUIRE(common::Vector{22, 54, -1}.length_squared() == 3401.0f);
}

static_assert(common::distance_squared({1, 1, 1}, {1, 1, 1}) == 0.0f);
static_assert(common::distance_squared({1, 1, 1}, {0, 1, 1}) == 1.0f);
static_assert(common::distance_squared({1, 1, 1}, {1, 0, 1}) == 1.0f);
static_assert(common::distance_squared({1, 1, 1}, {1, 1, 0}) == 1.0f);
static_assert(common::distance_squared({2, 2, 2}, {1, 1, 1}) == 3.0f);
static_assert(common::distance_squared({33, 27, 1}, {11, -27, 2}) == 3401.0f);

TEST_CASE("vector_to_vector_distance_squared", "[common::Vector]") {
  REQUIRE(common::distance_squared({1, 1, 1}, {1, 1, 1}) == 0.0f);
  REQUIRE(common::distance_squared({1, 1, 1}, {0, 1, 1}) == 1.0f);
  REQUIRE(common::distance_squared({1, 1, 1}, {1, 0, 1}) == 1.0f);
  REQUIRE(common::distance_squared({1, 1, 1}, {1, 1, 0}) == 1.0f);
  REQUIRE(common::distance_squared({2, 2, 2}, {1, 1, 1}) == 3.0f);
  REQUIRE(common::distance_squared({33, 27, 1}, {11, -27, 2}) == 3401.0f);
}

TEST_CASE("vector_to_vector_distance", "[common::Vector]") {
  REQUIRE(common::distance({1, 1, 1}, {1, 1, 1}) == Approx(0.0f));
  REQUIRE(common::distance({1, 1, 1}, {0, 1, 1}) == Approx(1.0f));
  REQUIRE(common::distance({1, 1, 1}, {1, 0, 1}) == Approx(1.0f));
  REQUIRE(common::distance({1, 1, 1}, {1, 1, 0}) == Approx(1.0f));
  REQUIRE(common::distance({2, 2, 2}, {1, 1, 1}) == Approx(1.7320508075f));
  REQUIRE(common::distance({33, 27, 1}, {11, -27, 2}) ==
          Approx(58.3180932472f));
}