#pragma once

#include <string>
#include <string_view>
#include <algorithm>

namespace common {
    inline bool icompare(std::string_view a, std::string_view b) {
        return std::equal(a.begin(), a.end(), b.begin(), b.end(),
                          [](auto ca, auto cb) { return ca == cb || std::toupper(ca) == std::toupper(ca); });
    }

    inline auto ifind(std::string_view haystack, std::string_view needle) {
        auto it = std::search(haystack.begin(), haystack.end(), needle.begin(), needle.end(),
                              [](auto ch1, auto ch2) { return ch1 == ch2 || std::toupper(ch1) == std::toupper(ch2); });
        return std::distance(haystack.begin(), it);
    }
}
