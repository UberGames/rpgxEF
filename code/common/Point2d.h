#pragma once

#include <cstdint>
#include <initializer_list>
#include <type_traits>
#include <utility>

namespace common {

template <typename T> class Point2d {
public:
  constexpr Point2d() : x_{0}, y_{0} {}

  template <typename A, typename B,
            typename = std::enable_if_t<std::is_convertible_v<A, T> &&
                                        std::is_convertible_v<B, T>>>
  constexpr Point2d(A x, B y) : x_{static_cast<T>(x)}, y_{static_cast<T>(y)} {}

  T x_;
  T y_;
};

using Point2dI = Point2d<int32_t>;
using Point2dF = Point2d<float>;
using Point2dD = Point2d<double>;

} // namespace common
