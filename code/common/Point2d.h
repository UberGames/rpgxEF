#pragma once

#include <cstdint>
#include <type_traits>

namespace common {

template <typename T> class Point2d {
public:
  template <typename A, typename B,
            typename = std::enable_if_t<std::is_convertible_v<A, T> &&
                                        std::is_convertible_v<B, T>>>
  constexpr explicit Point2d(A _x = 0, B _y = 0)
      : x{static_cast<T>(_x)}, y{static_cast<T>(_y)} {}

  T x;
  T y;
};

using Point2dI = Point2d<int32_t>;
using Point2dF = Point2d<float>;
using Point2dD = Point2d<double>;

} // namespace common
