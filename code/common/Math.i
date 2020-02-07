%module common
%{
#include "Math.h"
%}

namespace common {

double rsqrt(double number);

constexpr bool equal(double a, double b);
constexpr bool equal(double a, double b, double delta);

}; // namespace common