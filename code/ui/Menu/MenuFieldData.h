//
// Created by Walter on 08.02.2020.
//

#ifndef RPGXEF_CODE_UI_MENU_MENUFIELDDATA_H_
#define RPGXEF_CODE_UI_MENU_MENUFIELDDATA_H_

#include "MenuColor.h"
#include <cinttypes>
#include <string>

namespace ui {

struct MenuFieldData {
  std::int32_t cursor{0};
  std::int32_t scroll{0};
  std::int32_t width{0};
  std::size_t style{0};

  struct {
    std::int32_t id{0};
    MenuColor color;
  } title;

  struct {
    MenuColor color;
    std::size_t max_size{0};
    std::string buffer;
  } text;
};

} // namespace ui

#endif // RPGXEF_CODE_UI_MENU_MENUFIELDDATA_H_
