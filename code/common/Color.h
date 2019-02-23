#pragma once

#include <base_game/q_shared.h>

namespace common {
    class Color {
    public:
        constexpr Color() noexcept : r{0.f}, g{0.f}, b{0.f}, a{0.f} {}

        template<typename R, typename G, typename B, typename A, typename = std::enable_if_t<
                std::is_convertible_v<R, float> && std::is_convertible_v<G, float> && std::is_convertible_v<B, float> &&
                std::is_convertible_v<A, float>>>
        constexpr Color(R _r, G _g = 0, B _b = 0, A _a = 0) noexcept :
                r{_r}, g{_g}, b{_b}, a{_a} {}

        constexpr Color(vec4_t v) noexcept : r{v[0]}, g{v[1]}, b{v[2]}, a{v[3]} {}

        union {
            vec4_t values{};
            struct {
                float r;
                float g;
                float b;
                float a;
            };
        };
    };

}
