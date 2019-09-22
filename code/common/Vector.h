#pragma once

#include <array>
#include <base_game/q_shared.h>

namespace common {

class Vector {
public:
  constexpr Vector() noexcept : x_{0}, y_{0}, z_{0} {}

  template <typename X, typename Y = float, typename Z = float,
            typename = std::enable_if_t<std::is_convertible_v<X, float> &&
                                        std::is_convertible_v<Y, float> &&
                                        std::is_convertible_v<Z, float>>>
  constexpr Vector(X x, Y y, Z z) noexcept
      : x_{static_cast<float>(x)}, y_{static_cast<float>(y)},
        z_{static_cast<float>(z)} {}

  constexpr Vector(vec3_t v) noexcept : x_{v[0]}, y_{v[1]}, z_{v[2]} {}

  constexpr Vector &clear();

  [[nodiscard]] float length() const;

  [[nodiscard]] constexpr float length_squared() const;

  float normalize();

  void normalizeFast();

  [[nodiscard]] Vector normalized() const;

  [[nodiscard]] Vector normalizedFast() const;

  constexpr void invert();

  [[nodiscard]] Vector inverted() const;

  union {
    vec3_t values{};
    struct {
      float x_;
      float y_;
      float z_;
    };
  };
};

inline constexpr float dotProduct(const Vector &a, const Vector &b) {
  return a.x_ * b.x_ + a.y_ * b.y_ + a.z_ * b.z_;
}

inline constexpr Vector operator-(const Vector &a, const Vector &b) {
  return {a.x_ - b.x_, a.y_ - b.y_, a.z_ - b.z_};
}

inline constexpr Vector operator+(const Vector &a, const Vector &b) {
  return {a.x_ + b.x_, a.y_ + b.y_, a.z_ + b.z_};
}

inline constexpr Vector operator*(const Vector &a, float factor) {
  return {a.x_ * factor, a.y_ * factor, a.z_ * factor};
}

inline constexpr Vector operator*(float factor, const Vector &a) {
  return {a.x_ * factor, a.y_ * factor, a.z_ * factor};
}

inline constexpr Vector ma(const Vector &v, float s, const Vector &b) {
  return {v.x_ + b.x_ * s, v.y_ + b.y_ * s, v.z_ + b.z_ * s};
}

inline constexpr Vector operator-(const Vector &a) {
  return {-a.x_, -a.y_, -a.z_};
}

float distance(const Vector &a, const Vector &b);

constexpr float distance_squared(const Vector &a, const Vector &b);

inline constexpr Vector crossProduct(const Vector &v1, const Vector &v2) {
  return {v1.y_ * v2.z_ - v1.z_ * v2.y_, v1.z_ * v2.x_ - v1.x_ * v2.z_,
          v1.x_ * v2.y_ - v1.y_ * v2.x_};
}

float normalize2(const Vector &v, Vector &out);

constexpr Vector rotate(const Vector &v, const std::array<Vector, 3> &matrix) {
  return {dotProduct(v, matrix[0]), dotProduct(v, matrix[1]),
          dotProduct(v, matrix[2])};
}

std::int32_t dirToByte(const Vector& dir);

Vector byteToDir(std::int32_t b);

} // namespace common
