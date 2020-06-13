#pragma once
#include <type_traits>

namespace common {

template <
    typename A, typename B,
    typename = std::enable_if_t<std::is_integral_v<A> && std::is_integral_v<B>>>
constexpr bool bit_in(A bits, B bit) {
  return (bits & bit) == bit;
}

template <typename A, typename B, bool D1 = true,
          typename = std::enable_if_t<std::is_enum_v<A> &&
                                      std::is_integral_v<B> && D1>>
constexpr bool bit_in(A bits, B bit) {
  return (static_cast<std::underlying_type_t<A>>(bits) & bit) == bit;
}

template <typename A, typename B, bool D1 = true, bool D2 = true,
          typename = std::enable_if_t<std::is_integral_v<A> &&
                                      std::is_enum_v<B> && D1 && D2>>
constexpr bool bit_in(A bits, B bit) {
  return (bits & static_cast<std::underlying_type_t<B>>(bit)) == bit;
}

template <typename A, typename B, bool D1 = true, bool D2 = true,
          bool D3 = true,
          typename = std::enable_if_t<std::is_enum_v<A> && std::is_enum_v<B> &&
                                      D1 && D2 && D3>>
constexpr bool bit_in(A bits, B bit) {
  return (bits & bit) == bit;
}

} // namespace common
