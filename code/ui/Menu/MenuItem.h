//
// Created by Walter on 08.02.2020.
//

#ifndef RPGXEF_CODE_UI_MENU_MENUITEM_H_
#define RPGXEF_CODE_UI_MENU_MENUITEM_H_

#include "MenuCommon.h"

namespace ui {

struct MenuItem {
  MenuCommon common;

  virtual void init() = 0;
};

} // namespace ui

#endif // RPGXEF_CODE_UI_MENU_MENUITEM_H_
