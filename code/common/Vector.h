#pragma once

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

inline constexpr Vector operator*(float factor, Vector &a) {
  return {a.x_ * factor, a.y_ * factor, a.z_ * factor};
}

} // namespace common
