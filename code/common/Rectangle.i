%module common
%{
#include "Rectangle.h"
%}

namespace common {

class RectangleI {
public:
  RectangleI(int32_t left, int32_t top, int32_t width, double height);
  RectangleI(int32_t left, int32_t top, double width, int32_t height);
  RectangleI(int32_t left, int32_t top, double width, double height);
  RectangleI(int32_t left, double top, int32_t width, int32_t height);
  RectangleI(int32_t left, double top, int32_t width, double height);
  RectangleI(int32_t left, double top, double width, int32_t height);
  RectangleI(int32_t left, double top, double width, double height);
  RectangleI(double left, int32_t top, int32_t width, double height);
  RectangleI(double left, int32_t top, double width, int32_t height);
  RectangleI(double left, int32_t top, double width, double height);
  RectangleI(double left, double top, int32_t width, int32_t height);
  RectangleI(double left, double top, int32_t width, double height);
  RectangleI(double left, double top, double width, int32_t height);
  RectangleI(double left, double top, double width, double height);

  RectangleI(const RectangleI &);

  RectangleI &operator=(const RectangleI &);

  bool operator==(const RectangleI &);
  bool operator!=(const RectangleI &);

  int32_t width() const;
  int32_t height() const;
  int32_t area() const;

  int32_t left;
  int32_t top;
  int32_t right;
  int32_t bottom;
};

class RectangleD {
public:
  RectangleD(int32_t left, int32_t top, int32_t width, double height);
  RectangleD(int32_t left, int32_t top, double width, int32_t height);
  RectangleD(int32_t left, int32_t top, double width, double height);
  RectangleD(int32_t left, double top, int32_t width, int32_t height);
  RectangleD(int32_t left, double top, int32_t width, double height);
  RectangleD(int32_t left, double top, double width, int32_t height);
  RectangleD(int32_t left, double top, double width, double height);
  RectangleD(double left, int32_t top, int32_t width, double height);
  RectangleD(double left, int32_t top, double width, int32_t height);
  RectangleD(double left, int32_t top, double width, double height);
  RectangleD(double left, double top, int32_t width, int32_t height);
  RectangleD(double left, double top, int32_t width, double height);
  RectangleD(double left, double top, double width, int32_t height);
  RectangleD(double left, double top, double width, double height);

  RectangleD(const RectangleD &);

  RectangleI &operator=(const RectangleI &);

  bool operator==(const RectangleD &);
  bool operator!=(const RectangleD &);

  int32_t width() const;
  int32_t height() const;
  int32_t area() const;

  int32_t left;
  int32_t top;
  int32_t right;
  int32_t bottom;
};

bool intersect(const RectangleI &, const RectangleI &);
bool intersect(const RectangleI &, const RectangleD &);
bool intersect(const RectangleD &, const RectangleI &);
bool intersect(const RectangleD &, const RectangleD &);

RectangleI intersection(const RectangleI &a, const RectangleI &b);
RectangleD intersection(const RectangleD &a, const RectangleI &b);
RectangleI intersection(const RectangleI &a, const RectangleD &b);
RectangleD intersection(const RectangleD &a, const RectangleD &b);

RectangleI bounds(const RectangleI &a, const RectangleI &b);
RectangleI bounds(const RectangleI &a, const RectangleD &b);
RectangleD bounds(const RectangleD &a, const RectangleI &b);
RectangleD bounds(const RectangleD &a, const RectangleD &b);

} // namespace common