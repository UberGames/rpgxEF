//
// Created by Walter on 08.02.2020.
//

#ifndef RPGXEF_CODE_UI_MENU_MENURADIOBUTTON_H_
#define RPGXEF_CODE_UI_MENU_MENURADIOBUTTON_H_

#include "MenuItem.h"

namespace ui {

struct MenuRadioButton : MenuItem {
  std::size_t value{0};

  void init() override;
  void draw() override;
};

} // namespace ui

#endif // RPGXEF_CODE_UI_MENU_MENURADIOBUTTON_H_
