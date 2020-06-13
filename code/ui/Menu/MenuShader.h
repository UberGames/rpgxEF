//
// Created by Walter on 08.02.2020.
//

#ifndef RPGXEF_CODE_UI_MENU_MENUSHADER_H_
#define RPGXEF_CODE_UI_MENU_MENUSHADER_H_

#include <base_game/q_shared.h>
#include <string>

namespace ui {

struct MenuShader {
  std::string name;
  qhandle_t handle;

  operator bool() const { return !name.empty() && handle; }
};

} // namespace ui

#endif // RPGXEF_CODE_UI_MENU_MENUSHADER_H_
