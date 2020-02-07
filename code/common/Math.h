#pragma once

#include <cmath>

namespace common {

double rsqrt(double number);

constexpr bool equal(double a, double b, double delta = 0.0001) {
  return std::abs(a - b) <= delta;
}

}; // namespace common
