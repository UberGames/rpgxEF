//
// Created by Walter on 08.02.2020.
//

#ifndef RPGXEF_CODE_UI_MENU_MENUFIELD_H_
#define RPGXEF_CODE_UI_MENU_MENUFIELD_H_

#include "MenuCommon.h"
#include "MenuFieldData.h"
#include "MenuItem.h"

namespace ui {

struct MenuField : MenuItem {
  MenuFieldData data;

  void init() override;

  void clear();
};

} // namespace ui

#endif // RPGXEF_CODE_UI_MENU_MENUFIELD_H_
