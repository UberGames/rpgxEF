#pragma once

#include <array>
#include <base_game/q_shared.h>

namespace common {

class Vector {
public:
  constexpr Vector() noexcept : x{0}, y{0}, z{0} {}

  template <typename X, typename Y = float, typename Z = float,
            typename = std::enable_if_t<std::is_convertible_v<X, float> &&
                                        std::is_convertible_v<Y, float> &&
                                        std::is_convertible_v<Z, float>>>
  constexpr Vector(X x, Y y, Z z) noexcept
      : x{static_cast<float>(x)}, y{static_cast<float>(y)},
        z{static_cast<float>(z)} {}

  constexpr Vector(vec3_t v) noexcept : x{v[0]}, y{v[1]}, z{v[2]} {}

  constexpr Vector &clear() {
    x = y = z = 0;
    return *this;
  }

  [[nodiscard]] float length() const;

  [[nodiscard]] constexpr float length_squared() const {
    return x * x + y * y + z * z;
  }

  float normalize();

  void normalizeFast();

  [[nodiscard]] Vector normalized() const;

  [[nodiscard]] Vector normalizedFast() const;

  constexpr void invert() {
    values[0] = -values[0];
    values[1] = -values[1];
    values[2] = -values[2];
  }

  [[nodiscard]] Vector inverted() const;

  union {
    vec3_t values{};
    struct {
      float x;
      float y;
      float z;
    };
  };
};

inline constexpr float dotProduct(const Vector &a, const Vector &b) {
  return a.x * b.x + a.y * b.y + a.z * b.z;
}

inline constexpr Vector operator-(const Vector &a, const Vector &b) {
  return {a.x - b.x, a.y - b.y, a.z - b.z};
}

inline constexpr Vector operator+(const Vector &a, const Vector &b) {
  return {a.x + b.x, a.y + b.y, a.z + b.z};
}

inline constexpr Vector operator*(const Vector &a, float factor) {
  return {a.x * factor, a.y * factor, a.z * factor};
}

inline constexpr Vector operator*(float factor, const Vector &a) {
  return {a.x * factor, a.y * factor, a.z * factor};
}

inline constexpr Vector ma(const Vector &v, float s, const Vector &b) {
  return {v.x + b.x * s, v.y + b.y * s, v.z + b.z * s};
}

inline constexpr Vector operator-(const Vector &a) {
  return {-a.x, -a.y, -a.z};
}

float distance(const Vector &a, const Vector &b);

constexpr float distance_squared(const Vector &a, const Vector &b) {
  return dotProduct(b - a, b - a);
}

inline constexpr Vector crossProduct(const Vector &v1, const Vector &v2) {
  return {v1.y * v2.z - v1.z * v2.y, v1.z * v2.x - v1.x * v2.z,
          v1.x * v2.y - v1.y * v2.x};
}

float normalize2(const Vector &v, Vector &out);

constexpr Vector rotate(const Vector &v, const std::array<Vector, 3> &matrix) {
  return {dotProduct(v, matrix[0]), dotProduct(v, matrix[1]),
          dotProduct(v, matrix[2])};
}

std::int32_t dirToByte(const Vector &dir);

Vector byteToDir(std::int32_t b);

} // namespace common
