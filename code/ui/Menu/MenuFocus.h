//
// Created by Walter on 08.02.2020.
//

#ifndef RPGXEF_CODE_UI_MENU_MENUFOCUS_H_
#define RPGXEF_CODE_UI_MENU_MENUFOCUS_H_

#include "MenuSize.h"
#include <common/Point2d.h>

namespace ui {

struct MenuFocus {
  common::Point2dI position;
  MenuSize size;
};

}

#endif // RPGXEF_CODE_UI_MENU_MENUFOCUS_H_
