//
// Created by Walter on 08.02.2020.
//

#ifndef RPGXEF_CODE_UI_MENU_MENUTEXT_H_
#define RPGXEF_CODE_UI_MENU_MENUTEXT_H_

#include "MenuColor.h"
#include "MenuFocus.h"
#include "MenuItem.h"

namespace ui {

struct MenuText : MenuItem {
  std::string text;
  struct {
    std::size_t id{0};
    std::size_t id_2{0};
    std::size_t id_3{0};
  } text_normal;
  struct {
    std::size_t id{0};
    std::size_t id_2{0};
    std::size_t id_3{0};
  } text_button;
  std::size_t style{0};
  MenuColor color;
  MenuFocus focus;

  void init() override;
};

} // namespace ui

#endif // RPGXEF_CODE_UI_MENU_MENUTEXT_H_
