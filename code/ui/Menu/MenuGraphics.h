//
// Created by Walter on 08.02.2020.
//

#ifndef RPGXEF_CODE_UI_MENU_MENUGRAPHICS_H_
#define RPGXEF_CODE_UI_MENU_MENUGRAPHICS_H_

#include "MenuShader.h"
#include "MenuSize.h"
#include <cinttypes>
#include <common/Color.h>
#include <common/Point2d.h>

namespace ui {

struct MenuGraphics {
  std::size_t type{0};
  double timer{0};
  common::Point2dI position;
  MenuSize size;
  std::string text;
  MenuShader graphic;
  std::int32_t min{0};
  std::int32_t max{0};
  std::int32_t target{0};
  std::int32_t increment{0};
  std::size_t style{0};
  common::Color color;
};

} // namespace ui

#endif // RPGXEF_CODE_UI_MENU_MENUGRAPHICS_H_
