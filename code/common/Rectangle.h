#pragma once

#include <algorithm>
#include <cstdint>
#include <optional>
#include <tuple>
#include <type_traits>

namespace common {

template <typename T> class Rectangle {
public:
  constexpr Rectangle() : left{0}, top{0}, right{0}, bottom{0} {}

  template <typename A, typename B = A, typename C = A, typename D = A,
            typename = std::enable_if_t<
                std::is_convertible_v<A, T> && std::is_convertible_v<B, T> &&
                std::is_convertible_v<C, T> && std::is_convertible_v<D, T>>>
  constexpr explicit Rectangle(A _left, B _top = 0, C _right = 0, D _bottom = 0)
      : left{static_cast<T>(_left)}, top{static_cast<T>(_top)},
        right{static_cast<T>(_right)}, bottom{static_cast<T>(_bottom)} {}

  constexpr T width() const { return right - left; }
  constexpr T height() const { return bottom - top; }
  constexpr T area() const { return width() * height(); }

  constexpr bool operator==(const Rectangle &rhs) const {
    return std::tie(left, top, right, bottom) ==
           std::tie(rhs.left, rhs.top, rhs.right, rhs.bottom);
  }
  constexpr bool operator!=(const Rectangle &rhs) const {
    return !(rhs == *this);
  }

  T left;
  T top;
  T right;
  T bottom;
};

template <typename A, typename B>
bool intersect(const Rectangle<A> &a, const Rectangle<B> &b) {
  return !(a.left > b.right || a.right < b.left || a.top > b.bottom ||
           a.bottom < b.top);
}

template <typename A, typename B, typename ResultType = A>
std::optional<Rectangle<ResultType>> intersection(const Rectangle<A> &a,
                                                  const Rectangle<B> &b) {
  if (!intersect(a, b)) {
    return {};
  }

  auto left = std::max(a.left, b.left);
  auto bottom = std::min(a.bottom, b.bottom);
  auto right = std::min(a.right, b.right);
  auto top = std::max(a.top, b.top);

  return Rectangle<ResultType>(left, top, right, bottom);
}

template <typename A, typename... Args>
constexpr Rectangle<A> bounds(const Rectangle<A> &a, Args &&... args) {
  if constexpr (sizeof...(args) > 0) {
    auto b = bounds(std::forward<Args>(args)...);
    return Rectangle<A>{std::min(a.left, b.left), std::min(a.top, b.top),
                        std::max(a.right, b.right),
                        std::max(a.bottom, b.bottom)};
  }

  return a;
}

#if 0
template <typename A, typename B, typename... Args, typename ResultType = A>
Rectangle<ResultType> bounds(const Rectangle<A> &a, const Rectangle<B> &b,
                             Args &&... args) {
  auto bounds_a_b = bounds<A, B, ResultType>(a, b);
  auto bounds_args = bounds(std::forward<Args>(args)...);
  return bounds<ResultType, decltype(bounds_args), ResultType>(bounds_a_b,
                                                               bounds_args);
}

template <typename A, typename B, typename ResultType = A>
Rectangle<ResultType> bounds(const Rectangle<A> &a, const Rectangle<B> &b) {
  auto left = std::min(a.left, b.left);
  auto bottom = std::max(a.bottom, b.bottom);
  auto right = std::max(a.right, b.right);
  auto top = std::min(a.top, b.top);
  return Rectangle<ResultType>(left, top, right, bottom);
}

template <typename A, typename ResultType = A>
Rectangle<ResultType> bounds(const Rectangle<A> &a) {
  return a;
}
#endif

using RectangleI = Rectangle<int32_t>;
using RectangleF = Rectangle<float>;
using RectangleD = Rectangle<double>;

} // namespace common
