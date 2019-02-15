#pragma once

#include <cstdint>
#include <type_traits>

namespace common {

    template<typename T>
    class Rectangle {
    public:
        template<typename A, typename B, typename C, typename D, typename = std::enable_if_t<
                std::is_convertible_v<A, T> && std::is_convertible_v<B, T> && std::is_convertible_v<C, T> &&
                std::is_convertible_v<D, T>>>
        constexpr explicit
        Rectangle(A _left = 0, B _top = 0, C _right = 0, D _bottom = 0) : left{_left}, top{_top}, right{_right},
                                                                          bottom{_bottom} {}

        constexpr T width() const { return right - left; }
        constexpr T height() const { return bottom - top; }

        T left;
        T top;
        T right;
        T bottom;
    };

    using RectangleI = Rectangle<int32_t>;
    using RectangleF = Rectangle<float>;
    using RectangleD = Rectangle<double>;
}
