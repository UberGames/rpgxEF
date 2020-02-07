%module common
%{
#include "Color.h"
%}

namespace common {

class Color {
public:
  constexpr Color();
  constexpr explicit Color(float _r, float _g = 0, float _b = 0, float _a = 0);

  constexpr Color &operator=(const Color &);
  constexpr Color &operator=(Color &&);

  float r;
  float g;
  float b;
  float a;

  static const Color None;
  static const Color Black;
  static const Color Red;
  static const Color Green;
  static const Color Blue;
  static const Color Yellow;
  static const Color Magenta;
  static const Color Cyan;
  static const Color Teal;
  static const Color Gold;
  static const Color White;
  static const Color LightGrey;
  static const Color MediumGrey;
  static const Color DarkGrey;
  static const Color DarkGrey2;

  static const Color VeryLightOrange;
  static const Color LightOrange;
  static const Color DarkOrange;
  static const Color VeryDarkOrange;

  static const Color VeryLightBlue;
  static const Color LightBlue;
  static const Color DarkBlue;
  static const Color VeryDarkBlue;

  static const Color VeryLightBlue2;
  static const Color LightBlue2;
  static const Color DarkBlue2;
  static const Color VeryDarkBlue2;

  static const Color VeryLightBrown;
  static const Color LightBrown;
  static const Color DarkBrown;
  static const Color VeryDarkBrown;

  static const Color VeryLightGold;
  static const Color LightGold;
  static const Color DarkGold;
  static const Color VeryDarkGold;

  static const Color VeryLightPurple;
  static const Color LightPurple;
  static const Color DarkPurple;
  static const Color VeryDarkPurple;

  static const Color VeryLightPurple2;
  static const Color LightPurple2;
  static const Color DarkPurple2;
  static const Color VeryDarkPurple2;

  static const Color VeryLightPurple3;
  static const Color LightPurple3;
  static const Color DarkPurple3;
  static const Color VeryDarkPurple3;

  static const Color VeryLightRed;
  static const Color LightRed;
  static const Color DarkRed;
  static const Color VeryDarkRed;
};

} // namespace common