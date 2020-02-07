#include "Vector.h"
#include "Math.h"
#include "Random.h"
#include <base_game/q_math.h>
#include <catch2/catch.hpp>

namespace common {

float Vector::length() const { return sqrt(dotProduct(*this, *this)); }

float Vector::normalize() {
  const auto len = length();

  if (len < 0 || len > 0) {
    const auto ilen = 1 / len;
    x *= ilen;
    y *= ilen;
    z *= ilen;
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
  x *= ilen;
  y *= ilen;
  z *= ilen;
}

Vector Vector::normalizedFast() const {
  auto result = *this;
  result.normalizeFast();
  return result;
}

Vector Vector::inverted() const {
  auto result = *this;
  result.invert();
  return result;
}

float distance(const Vector &a, const Vector &b) {
  return sqrt(dotProduct(b - a, b - a));
}

float normalize2(const Vector &v, Vector &out) {
  const auto len = v.length();

  if (len < 0 || len > 0) {
    const auto ilen = 1 / len;
    out.x = v.x * ilen;
    out.y = v.y * ilen;
    out.z = v.z * ilen;
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

static_assert(common::Vector().x == 0.0f);
static_assert(common::Vector().y == 0.0f);
static_assert(common::Vector().z == 0.0f);

static_assert(common::Vector(1, 2, 3).x == 1.0f);
static_assert(common::Vector(1, 2, 3).y == 2.0f);
static_assert(common::Vector(1, 2, 3).z == 3.0f);

static_assert(common::Vector(1.0f, 2.5f, -3.123f).x == 1.0f);
static_assert(common::Vector(1.0f, 2.5f, -3.123f).y == 2.5f);
static_assert(common::Vector(1.0f, 2.5f, -3.123f).z == -3.123f);

static_assert(common::Vector(1.0, 2.5, -3.123).x == 1.0f);
static_assert(common::Vector(1.0, 2.5, -3.123).y == 2.5f);
static_assert(common::Vector(1.0, 2.5, -3.123).z == -3.123f);

TEST_CASE("vector_construct", "[common::Vector]") {
  REQUIRE(common::Vector().x == Approx(0.0f));
  REQUIRE(common::Vector().y == Approx(0.0f));
  REQUIRE(common::Vector().z == Approx(0.0f));

  REQUIRE(common::Vector(1, 2, 3).x == Approx(1.0f));
  REQUIRE(common::Vector(1, 2, 3).y == Approx(2.0f));
  REQUIRE(common::Vector(1, 2, 3).z == Approx(3.0f));

  REQUIRE(common::Vector(1.0f, 2.5f, -3.123f).x == Approx(1.0f));
  REQUIRE(common::Vector(1.0f, 2.5f, -3.123f).y == Approx(2.5f));
  REQUIRE(common::Vector(1.0f, 2.5f, -3.123f).z == Approx(-3.123f));

  REQUIRE(common::Vector(1.0, 2.5, -3.123).x == Approx(1.0f));
  REQUIRE(common::Vector(1.0, 2.5, -3.123).y == Approx(2.5f));
  REQUIRE(common::Vector(1.0, 2.5, -3.123).z == Approx(-3.123f));

  vec3_t vec_3{1, 2, 3};
  REQUIRE(common::Vector(vec_3).x == Approx(1.0f));
  REQUIRE(common::Vector(vec_3).y == Approx(2.0f));
  REQUIRE(common::Vector(vec_3).z == Approx(3.0f));
}

static_assert(common::dotProduct({1, 2, 3}, {3, 2, 1}) == 10.0f);
static_assert(common::dotProduct({1, 5, 3}, {3, 2, 1}) == 16.0f);

TEST_CASE("vector_dotproduct", "[common::Vector]") {
  REQUIRE(common::dotProduct({1, 2, 3}, {3, 2, 1}) == Approx(10.0f));
  REQUIRE(common::dotProduct({1, 5, 3}, {3, 2, 1}) == Approx(16.0f));

  for (auto i = 0; i < 100; i++) {
    auto v1 = common::Vector{common::getRandomDouble<float>(-100000, 100000),
                             common::getRandomDouble<float>(-100000, 100000),
                             common::getRandomDouble<float>(-100000, 100000)};
    auto v2 = common::Vector{common::getRandomDouble<float>(-100000, 100000),
                             common::getRandomDouble<float>(-100000, 100000),
                             common::getRandomDouble<float>(-100000, 100000)};

    REQUIRE(common::dotProduct(v1, v2) ==
            Approx(v1.x * v2.x + v1.y * v2.y + v1.z * v2.z));
  }
}

static_assert((common::Vector{2, 5, 4} - common::Vector{1, -2, 4}).x == 1.0f);
static_assert((common::Vector{2, 5, 4} - common::Vector{1, -2, 4}).y == 7.0f);
static_assert((common::Vector{2, 5, 4} - common::Vector{1, -2, 4}).z == 0.0f);

TEST_CASE("vector_substract", "[common::Vector}") {
  REQUIRE((common::Vector{2, 5, 4} - common::Vector{1, -2, 4}).x == 1.0f);
  REQUIRE((common::Vector{2, 5, 4} - common::Vector{1, -2, 4}).y == 7.0f);
  REQUIRE((common::Vector{2, 5, 4} - common::Vector{1, -2, 4}).z == 0.0f);

  for (auto i = 0; i < 100; i++) {
    auto v1 = common::Vector{common::getRandomDouble<float>(-100000, 100000),
                             common::getRandomDouble<float>(-100000, 100000),
                             common::getRandomDouble<float>(-100000, 100000)};
    auto v2 = common::Vector{common::getRandomDouble<float>(-100000, 100000),
                             common::getRandomDouble<float>(-100000, 100000),
                             common::getRandomDouble<float>(-100000, 100000)};

    auto res = v1 - v2;
    REQUIRE(res.x == v1.x - v2.x);
    REQUIRE(res.y == v1.y - v2.y);
    REQUIRE(res.z == v1.z - v2.z);
  }
}

static_assert((common::Vector{2, 5, 4} + common::Vector{1, -2, 4}).x == 3.0f);
static_assert((common::Vector{2, 5, 4} + common::Vector{1, -2, 4}).y == 3.0f);
static_assert((common::Vector{2, 5, 4} + common::Vector{1, -2, 4}).z == 8.0f);

TEST_CASE("vector_add", "[common::Vector]") {
  REQUIRE((common::Vector{2, 5, 4} + common::Vector{1, -2, 4}).x == 3.0f);
  REQUIRE((common::Vector{2, 5, 4} + common::Vector{1, -2, 4}).y == 3.0f);
  REQUIRE((common::Vector{2, 5, 4} + common::Vector{1, -2, 4}).z == 8.0f);

  for (auto i = 0; i < 100; i++) {
    auto v1 = common::Vector{common::getRandomDouble<float>(-100000, 100000),
                             common::getRandomDouble<float>(-100000, 100000),
                             common::getRandomDouble<float>(-100000, 100000)};
    auto v2 = common::Vector{common::getRandomDouble<float>(-100000, 100000),
                             common::getRandomDouble<float>(-100000, 100000),
                             common::getRandomDouble<float>(-100000, 100000)};

    auto res = v1 + v2;
    REQUIRE(res.x == v1.x + v2.x);
    REQUIRE(res.y == v1.y + v2.y);
    REQUIRE(res.z == v1.z + v2.z);
  }
}

static_assert(common::Vector{1, 2, 3}.clear().x == 0.0f);
static_assert(common::Vector{1, 2, 3}.clear().y == 0.0f);
static_assert(common::Vector{1, 2, 3}.clear().z == 0.0f);

TEST_CASE("vector_clear", "[common::Vector]") {
  REQUIRE(common::Vector{1, 2, 3}.clear().x == 0.0f);
  REQUIRE(common::Vector{1, 2, 3}.clear().y == 0.0f);
  REQUIRE(common::Vector{1, 2, 3}.clear().z == 0.0f);
}

static_assert(common::ma(common::Vector{1, 2, 3}, 2, common::Vector{4, 5, 6})
                  .x == 9.00f);
static_assert(common::ma(common::Vector{1, 2, 3}, 2, common::Vector{4, 5, 6})
                  .y == 12.0f);
static_assert(common::ma(common::Vector{1, 2, 3}, 2, common::Vector{4, 5, 6})
                  .z == 15.0f);

TEST_CASE("vector_ma", "[common::Vector]") {
  REQUIRE(common::ma(common::Vector{1, 2, 3}, 2, common::Vector{4, 5, 6}).x ==
          9.00f);
  REQUIRE(common::ma(common::Vector{1, 2, 3}, 2, common::Vector{4, 5, 6}).y ==
          12.0f);
  REQUIRE(common::ma(common::Vector{1, 2, 3}, 2, common::Vector{4, 5, 6}).z ==
          15.0f);
}

static_assert((-common::Vector{1, 2, -5}).x == -1.0f);
static_assert((-common::Vector{1, 2, -5}).y == -2.0f);
static_assert((-common::Vector{1, 2, -5}).z == 5.0f);

static_assert((-common::Vector{-1, -2, 5}).x == 1.0f);
static_assert((-common::Vector{-1, -2, 5}).y == 2.0f);
static_assert((-common::Vector{-1, -2, 5}).z == -5.0f);

TEST_CASE("vector_negate", "[common::Vector]") {
  REQUIRE((-common::Vector{1, 2, -5}).x == -1.0f);
  REQUIRE((-common::Vector{1, 2, -5}).y == -2.0f);
  REQUIRE((-common::Vector{1, 2, -5}).z == 5.0f);

  REQUIRE((-common::Vector{-1, -2, 5}).x == 1.0f);
  REQUIRE((-common::Vector{-1, -2, 5}).y == 2.0f);
  REQUIRE((-common::Vector{-1, -2, 5}).z == -5.0f);
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

static_assert(common::crossProduct({1, 2, 3}, {4, 5, 6}).x == -3.0f);
static_assert(common::crossProduct({1, 2, 3}, {4, 5, 6}).y == 6.0f);
static_assert(common::crossProduct({1, 2, 3}, {4, 5, 6}).z == -3.0f);

TEST_CASE("vector_cross_product", "[common::Vector]") {
  REQUIRE(common::crossProduct({1, 2, 3}, {4, 5, 6}).x == -3.0f);
  REQUIRE(common::crossProduct({1, 2, 3}, {4, 5, 6}).y == 6.0f);
  REQUIRE(common::crossProduct({1, 2, 3}, {4, 5, 6}).z == -3.0f);
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
  REQUIRE(vec.x == Approx(-1.0f));
  REQUIRE(vec.y == Approx(1.0f));
  REQUIRE(vec.z == Approx(-1.0f));
}

TEST_CASE("vector_inverted", "[common::Vector]") {
  REQUIRE(common::Vector{1, -1, 1}.inverted().x == Approx(-1.0f));
  REQUIRE(common::Vector{1, -1, 1}.inverted().y == Approx(1.0f));
  REQUIRE(common::Vector{1, -1, 1}.inverted().z == Approx(-1.0f));
}

TEST_CASE("vector_rotate", "[common::Vector]") {
  auto vec = common::Vector{15, 2, -4};
  auto rotated = common::rotate(
      vec, std::array<common::Vector, 3>{
               common::Vector{4.5, 3, -6}, {1, 0, 7.6}, {1.3, -65, 7.5}});
  REQUIRE(rotated.x == Approx(97.5));
  REQUIRE(rotated.y == Approx(-15.4));
  REQUIRE(rotated.z == Approx(-140.5));
}

TEST_CASE("byte_to_dir", "[common::Vector]") {
  // TODO write unit test for common::byteToDir
}

TEST_CASE("dir_to_byte", "[common::Vector]") {
  // TODO write unit test for common::dirToByte
}
