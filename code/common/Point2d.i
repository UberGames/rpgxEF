%module common
%{
#include "Point2d.h"
%}

namespace common {

class Point2dI {
public:
  Point2dI(int32_t x, int32_t y);
  Point2dI(double x, double y);
  Point2dI(int32_t x, double y);
  Point2dI(double y, int32_t);

  int x_;
  int y_;
};

class Point2dD {
public:
  Point2dD(int32_t x, int32_t y);
  Point2dD(double x, double y);
  Point2dD(int32_t x, double y);
  Point2dD(double y, int32_t);

  double x_;
  double y_;
};

} // namespace common
