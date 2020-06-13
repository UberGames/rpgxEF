//
// Created by Walter on 08.02.2020.
//

#ifndef RPGXEF_CODE_UI_MENU_MENUACTION_H_
#define RPGXEF_CODE_UI_MENU_MENUACTION_H_

#include "MenuColor.h"
#include "MenuItem.h"
#include "MenuSize.h"

namespace ui {

struct MenuAction : MenuItem {
  MenuColor color;
  struct {
    std::size_t id{0};
    std::size_t id2{0};
    common::Point2dI position;
    MenuColor color;
  } text;
  MenuSize size;
  bool updated{false};

  void init() override;
  void draw() override;
};

} // namespace ui

#endif // RPGXEF_CODE_UI_MENU_MENUACTION_H_
