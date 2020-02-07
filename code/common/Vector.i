%module common
%{
#include "Vector.h"
%}

namespace common {

class Vector {
public:
  constexpr Vector();
  constexpr Vector(float x, float y, float z);

  constexpr Vector &clear();

  float length() const;

  constexpr float length_squared() const;

  float normalize();

  void normalizeFast();

  Vector normalized() const;

  Vector normalizedFast() const;

  constexpr void invert();

  Vector inverted() const;

  float x;
  float y;
  float z;
};

constexpr float dotProduct(const Vector &a, const Vector &b);

constexpr Vector operator-(const Vector &a, const Vector &b);

constexpr Vector operator+(const Vector &a, const Vector &b);

constexpr Vector operator*(const Vector &a, float factor);

constexpr Vector operator*(float factor, const Vector &a);

constexpr Vector ma(const Vector &v, float s, const Vector &b);

constexpr Vector operator-(const Vector &a);

float distance(const Vector &a, const Vector &b);

constexpr float distance_squared(const Vector &a, const Vector &b);

constexpr Vector crossProduct(const Vector &v1, const Vector &v2);

float normalize2(const Vector &v, Vector &out);

Vector rotate(const Vector &v, const std::array<Vector, 3> &matrix);

std::int32_t dirToByte(const Vector &dir);

Vector byteToDir(std::int32_t b);

} // namespace common