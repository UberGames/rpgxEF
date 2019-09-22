#include "Vector.h"
#include "Math.h"
#include "Random.h"
#include <base_game/q_math.h>
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

float Vector::normalize() {
  const auto len = length();

  if (len < 0 || len > 0) {
    const auto ilen = 1 / len;
    x_ *= ilen;
    y_ *= ilen;
    z_ *= ilen;
  }

  return len;
}

Vector Vector::normalized() const {
  auto result = *this;
  result.normalize();
  return result;
}

void Vector::normalizeFast() {
  const auto ilen = common::rsqrt(length_squared());
  x_ *= ilen;
  y_ *= ilen;
  z_ *= ilen;
}

Vector Vector::normalizedFast() const {
  auto result = *this;
  result.normalizeFast();
  return result;
}

constexpr void Vector::invert() {
  x_ = -x_;
  y_ = -y_;
  z_ = -z_;
}

Vector Vector::inverted() const {
  auto result = *this;
  result.invert();
  return result;
}

float distance(const Vector &a, const Vector &b) {
  return sqrt(dotProduct(b - a, b - a));
}

constexpr float distance_squared(const Vector &a, const Vector &b) {
  return dotProduct(b - a, b - a);
}

float normalize2(const Vector &v, Vector &out) {
  const auto len = v.length();

  if (len < 0 || len > 0) {
    const auto ilen = 1 / len;
    out.x_ = v.x_ * ilen;
    out.y_ = v.y_ * ilen;
    out.z_ = v.z_ * ilen;
  } else {
    out.clear();
  }

  return len;
}

constexpr std::array<Vector, 162> BYTE_TO_DIRS{
    common::Vector{-0.525731f, 0.000000f, 0.850651f},
    {-0.442863f, 0.238856f, 0.864188f},
    {-0.295242f, 0.000000f, 0.955423f},
    {-0.309017f, 0.500000f, 0.809017f},
    {-0.162460f, 0.262866f, 0.951056f},
    {0.000000f, 0.000000f, 1.000000f},
    {0.000000f, 0.850651f, 0.525731f},
    {-0.147621f, 0.716567f, 0.681718f},
    {0.147621f, 0.716567f, 0.681718f},
    {0.000000f, 0.525731f, 0.850651f},
    {0.309017f, 0.500000f, 0.809017f},
    {0.525731f, 0.000000f, 0.850651f},
    {0.295242f, 0.000000f, 0.955423f},
    {0.442863f, 0.238856f, 0.864188f},
    {0.162460f, 0.262866f, 0.951056f},
    {-0.681718f, 0.147621f, 0.716567f},
    {-0.809017f, 0.309017f, 0.500000f},
    {-0.587785f, 0.425325f, 0.688191f},
    {-0.850651f, 0.525731f, 0.000000f},
    {-0.864188f, 0.442863f, 0.238856f},
    {-0.716567f, 0.681718f, 0.147621f},
    {-0.688191f, 0.587785f, 0.425325f},
    {-0.500000f, 0.809017f, 0.309017f},
    {-0.238856f, 0.864188f, 0.442863f},
    {-0.425325f, 0.688191f, 0.587785f},
    {-0.716567f, 0.681718f, -0.147621f},
    {-0.500000f, 0.809017f, -0.309017f},
    {-0.525731f, 0.850651f, 0.000000f},
    {0.000000f, 0.850651f, -0.525731f},
    {-0.238856f, 0.864188f, -0.442863f},
    {0.000000f, 0.955423f, -0.295242f},
    {-0.262866f, 0.951056f, -0.162460f},
    {0.000000f, 1.000000f, 0.000000f},
    {0.000000f, 0.955423f, 0.295242f},
    {-0.262866f, 0.951056f, 0.162460f},
    {0.238856f, 0.864188f, 0.442863f},
    {0.262866f, 0.951056f, 0.162460f},
    {0.500000f, 0.809017f, 0.309017f},
    {0.238856f, 0.864188f, -0.442863f},
    {0.262866f, 0.951056f, -0.162460f},
    {0.500000f, 0.809017f, -0.309017f},
    {0.850651f, 0.525731f, 0.000000f},
    {0.716567f, 0.681718f, 0.147621f},
    {0.716567f, 0.681718f, -0.147621f},
    {0.525731f, 0.850651f, 0.000000f},
    {0.425325f, 0.688191f, 0.587785f},
    {0.864188f, 0.442863f, 0.238856f},
    {0.688191f, 0.587785f, 0.425325f},
    {0.809017f, 0.309017f, 0.500000f},
    {0.681718f, 0.147621f, 0.716567f},
    {0.587785f, 0.425325f, 0.688191f},
    {0.955423f, 0.295242f, 0.000000f},
    {1.000000f, 0.000000f, 0.000000f},
    {0.951056f, 0.162460f, 0.262866f},
    {0.850651f, -0.525731f, 0.000000f},
    {0.955423f, -0.295242f, 0.000000f},
    {0.864188f, -0.442863f, 0.238856f},
    {0.951056f, -0.162460f, 0.262866f},
    {0.809017f, -0.309017f, 0.500000f},
    {0.681718f, -0.147621f, 0.716567f},
    {0.850651f, 0.000000f, 0.525731f},
    {0.864188f, 0.442863f, -0.238856f},
    {0.809017f, 0.309017f, -0.500000f},
    {0.951056f, 0.162460f, -0.262866f},
    {0.525731f, 0.000000f, -0.850651f},
    {0.681718f, 0.147621f, -0.716567f},
    {0.681718f, -0.147621f, -0.716567f},
    {0.850651f, 0.000000f, -0.525731f},
    {0.809017f, -0.309017f, -0.500000f},
    {0.864188f, -0.442863f, -0.238856f},
    {0.951056f, -0.162460f, -0.262866f},
    {0.147621f, 0.716567f, -0.681718f},
    {0.309017f, 0.500000f, -0.809017f},
    {0.425325f, 0.688191f, -0.587785f},
    {0.442863f, 0.238856f, -0.864188f},
    {0.587785f, 0.425325f, -0.688191f},
    {0.688191f, 0.587785f, -0.425325f},
    {-0.147621f, 0.716567f, -0.681718f},
    {-0.309017f, 0.500000f, -0.809017f},
    {0.000000f, 0.525731f, -0.850651f},
    {-0.525731f, 0.000000f, -0.850651f},
    {-0.442863f, 0.238856f, -0.864188f},
    {-0.295242f, 0.000000f, -0.955423f},
    {-0.162460f, 0.262866f, -0.951056f},
    {0.000000f, 0.000000f, -1.000000f},
    {0.295242f, 0.000000f, -0.955423f},
    {0.162460f, 0.262866f, -0.951056f},
    {-0.442863f, -0.238856f, -0.864188f},
    {-0.309017f, -0.500000f, -0.809017f},
    {-0.162460f, -0.262866f, -0.951056f},
    {0.000000f, -0.850651f, -0.525731f},
    {-0.147621f, -0.716567f, -0.681718f},
    {0.147621f, -0.716567f, -0.681718f},
    {0.000000f, -0.525731f, -0.850651f},
    {0.309017f, -0.500000f, -0.809017f},
    {0.442863f, -0.238856f, -0.864188f},
    {0.162460f, -0.262866f, -0.951056f},
    {0.238856f, -0.864188f, -0.442863f},
    {0.500000f, -0.809017f, -0.309017f},
    {0.425325f, -0.688191f, -0.587785f},
    {0.716567f, -0.681718f, -0.147621f},
    {0.688191f, -0.587785f, -0.425325f},
    {0.587785f, -0.425325f, -0.688191f},
    {0.000000f, -0.955423f, -0.295242f},
    {0.000000f, -1.000000f, 0.000000f},
    {0.262866f, -0.951056f, -0.162460f},
    {0.000000f, -0.850651f, 0.525731f},
    {0.000000f, -0.955423f, 0.295242f},
    {0.238856f, -0.864188f, 0.442863f},
    {0.262866f, -0.951056f, 0.162460f},
    {0.500000f, -0.809017f, 0.309017f},
    {0.716567f, -0.681718f, 0.147621f},
    {0.525731f, -0.850651f, 0.000000f},
    {-0.238856f, -0.864188f, -0.442863f},
    {-0.500000f, -0.809017f, -0.309017f},
    {-0.262866f, -0.951056f, -0.162460f},
    {-0.850651f, -0.525731f, 0.000000f},
    {-0.716567f, -0.681718f, -0.147621f},
    {-0.716567f, -0.681718f, 0.147621f},
    {-0.525731f, -0.850651f, 0.000000f},
    {-0.500000f, -0.809017f, 0.309017f},
    {-0.238856f, -0.864188f, 0.442863f},
    {-0.262866f, -0.951056f, 0.162460f},
    {-0.864188f, -0.442863f, 0.238856f},
    {-0.809017f, -0.309017f, 0.500000f},
    {-0.688191f, -0.587785f, 0.425325f},
    {-0.681718f, -0.147621f, 0.716567f},
    {-0.442863f, -0.238856f, 0.864188f},
    {-0.587785f, -0.425325f, 0.688191f},
    {-0.309017f, -0.500000f, 0.809017f},
    {-0.147621f, -0.716567f, 0.681718f},
    {-0.425325f, -0.688191f, 0.587785f},
    {-0.162460f, -0.262866f, 0.951056f},
    {0.442863f, -0.238856f, 0.864188f},
    {0.162460f, -0.262866f, 0.951056f},
    {0.309017f, -0.500000f, 0.809017f},
    {0.147621f, -0.716567f, 0.681718f},
    {0.000000f, -0.525731f, 0.850651f},
    {0.425325f, -0.688191f, 0.587785f},
    {0.587785f, -0.425325f, 0.688191f},
    {0.688191f, -0.587785f, 0.425325f},
    {-0.955423f, 0.295242f, 0.000000f},
    {-0.951056f, 0.162460f, 0.262866f},
    {-1.000000f, 0.000000f, 0.000000f},
    {-0.850651f, 0.000000f, 0.525731f},
    {-0.955423f, -0.295242f, 0.000000f},
    {-0.951056f, -0.162460f, 0.262866f},
    {-0.864188f, 0.442863f, -0.238856f},
    {-0.951056f, 0.162460f, -0.262866f},
    {-0.809017f, 0.309017f, -0.500000f},
    {-0.864188f, -0.442863f, -0.238856f},
    {-0.951056f, -0.162460f, -0.262866f},
    {-0.809017f, -0.309017f, -0.500000f},
    {-0.681718f, 0.147621f, -0.716567f},
    {-0.681718f, -0.147621f, -0.716567f},
    {-0.850651f, 0.000000f, -0.525731f},
    {-0.688191f, 0.587785f, -0.425325f},
    {-0.587785f, 0.425325f, -0.688191f},
    {-0.425325f, 0.688191f, -0.587785f},
    {-0.425325f, -0.688191f, -0.587785f},
    {-0.587785f, -0.425325f, -0.688191f},
    {-0.688191f, -0.587785f, -0.425325f}};

std::int32_t dirToByte(const Vector &dir) {
  auto bestd = 0.0f;
  auto best = 0;
  for (auto i = 0; i < BYTE_TO_DIRS.size(); i++) {
    auto d = dotProduct(dir, BYTE_TO_DIRS[i]);
    if (d > bestd) {
      bestd = d;
      best = i;
    }
  }

  return best;
}

Vector byteToDir(std::int32_t b) {
  if (b < 0 || b >= BYTE_TO_DIRS.size()) {
    return {};
  }
  return BYTE_TO_DIRS[b];
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

static_assert(common::crossProduct({1, 2, 3}, {4, 5, 6}).x_ == -3.0f);
static_assert(common::crossProduct({1, 2, 3}, {4, 5, 6}).y_ == 6.0f);
static_assert(common::crossProduct({1, 2, 3}, {4, 5, 6}).z_ == -3.0f);

TEST_CASE("vector_cross_product", "[common::Vector]") {
  REQUIRE(common::crossProduct({1, 2, 3}, {4, 5, 6}).x_ == -3.0f);
  REQUIRE(common::crossProduct({1, 2, 3}, {4, 5, 6}).y_ == 6.0f);
  REQUIRE(common::crossProduct({1, 2, 3}, {4, 5, 6}).z_ == -3.0f);
}

TEST_CASE("vector_normalize", "[common::Vector]") {
  auto vec = common::Vector{4, -1, 7};
  auto res = vec.normalize();
  REQUIRE(vec.length() == Approx(1.0f));
  REQUIRE(res == Approx(8.1240384046f));
}

TEST_CASE("vector_normalize_fast", "[common::Vector]") {
  auto vec = common::Vector{4, -1, 7};
  vec.normalizeFast();
  REQUIRE(vec.length() == Approx(1.0f).epsilon(0.01));
}

TEST_CASE("vector_normalized", "[common::Vector]") {
  auto vec = common::Vector{4, -1, 7};
  REQUIRE(vec.normalized().length() == Approx(1.0f));
}

TEST_CASE("vector_normalized_fast", "[common::Vector]") {
  auto vec = common::Vector{4, -1, 7};
  REQUIRE(vec.normalizedFast().length() == Approx(1.0f).epsilon(0.01));
}

TEST_CASE("vector_normalize2", "[common::Vector]") {
  auto vec = common::Vector{4, -1, 7};
  auto vecr = common::Vector{};
  auto res = common::normalize2(vec, vecr);
  REQUIRE(res == Approx(8.1240384046f));
  REQUIRE(vecr.length() == Approx(1.0f));
}

TEST_CASE("vector_invert", "[common::Vector]") {
  auto vec = common::Vector{1, -1, 1};
  vec.invert();
  REQUIRE(vec.x_ == Approx(-1.0f));
  REQUIRE(vec.y_ == Approx(1.0f));
  REQUIRE(vec.z_ == Approx(-1.0f));
}

TEST_CASE("vector_inverted", "[common::Vector]") {
  REQUIRE(common::Vector{1, -1, 1}.inverted().x_ == Approx(-1.0f));
  REQUIRE(common::Vector{1, -1, 1}.inverted().y_ == Approx(1.0f));
  REQUIRE(common::Vector{1, -1, 1}.inverted().z_ == Approx(-1.0f));
}

TEST_CASE("vector_rotate", "[common::Vector]") {
  auto vec = common::Vector{15, 2, -4};
  auto rotated = common::rotate(
      vec, std::array<common::Vector, 3>{
               common::Vector{4.5, 3, -6}, {1, 0, 7.6}, {1.3, -65, 7.5}});
  REQUIRE(rotated.x_ == Approx(97.5));
  REQUIRE(rotated.y_ == Approx(-15.4));
  REQUIRE(rotated.z_ == Approx(-140.5));
}

TEST_CASE("byte_to_dir", "[common::Vector]") {
  // TODO write unit test for common::byteToDir
}

TEST_CASE("dir_to_byte", "[common::Vector]") {
  // TODO write unit test for common::dirToByte
}
