//
// Created by Walter on 08.02.2020.
//

#ifndef RPGXEF_CODE_UI_MENU_MENUSLIDER_H_
#define RPGXEF_CODE_UI_MENU_MENUSLIDER_H_

#include "MenuColor.h"
#include "MenuCommon.h"
#include "MenuFocus.h"
#include "MenuItem.h"
#include "MenuSize.h"
#include <common/Point2d.h>

namespace ui {

struct MenuSlider : MenuItem {
  struct {
    double min{0};
    double max{0};
    double current{0};
    double def{0};
  } value;

  MenuFocus focus;
  MenuColor color;

  struct {
    std::int32_t shader{0};
    MenuSize size;
  } graph;

  struct {
    std::string name;
    std::int32_t shader{0};
    MenuColor color;
    MenuSize size;
    std::int32_t graphic_width{0};
  } thumb;

  struct {
    std::string name;
    std::int32_t shader{0};
    MenuSize size;
    common::Point2dI position;
  } picture;

  struct {
    std::int32_t id{0};
    common::Point2dI position;
    MenuColor color;
  } text;

  double range{0};
  bool mouse_down{false};

  void init() override;
  void draw() override;
};

} // namespace ui

#endif // RPGXEF_CODE_UI_MENU_MENUSLIDER_H_
