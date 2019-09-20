#pragma once

#include <base_game/q_shared.h>

namespace common {
namespace detail {

template <typename R,
          typename = std::enable_if_t<std::is_convertible_v<R, float>>>
static constexpr float color_check_bounds(R value) {
  return value < 0.0f ? 0.0f
                      : value > 255.f ? 255.0f : static_cast<float>(value);
}

} // namespace detail

class Color {
public:
  constexpr Color() noexcept : r_{0.f}, g_{0.f}, b_{0.f}, a_{0.f} {}

  template <
      typename R, typename G = float, typename B = float, typename A = float,
      typename = std::enable_if_t<
          std::is_convertible_v<R, float> && std::is_convertible_v<G, float> &&
          std::is_convertible_v<B, float> && std::is_convertible_v<A, float>>>
  constexpr explicit Color(R _r, G _g = 0, B _b = 0, A _a = 0) noexcept
      : r_{detail::color_check_bounds(_r)}, g_{detail::color_check_bounds(_g)},
        b_{detail::color_check_bounds(_b)}, a_{detail::color_check_bounds(_a)} {
  }

  constexpr Color(vec4_t v) noexcept : r_{v[0]}, g_{v[1]}, b_{v[2]}, a_{v[3]} {}

  union {
    vec4_t values{};
    struct {
      float r_;
      float g_;
      float b_;
      float a_;
    };
  };
};

} // namespace common
