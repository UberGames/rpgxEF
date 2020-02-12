//
// Created by Walter on 08.02.2020.
//

#include "MenuRadioButton.h"
#include <base_game/q_shared.h>

namespace ui {

void MenuRadioButton::init() {
  auto length = common.name.empty() ? 0 : common.name.length();
  common.rectangle.left = common.position.x - (length + 1) * SMALLCHAR_WIDTH;
  common.rectangle.right = common.position.x + 6 * SMALLCHAR_WIDTH;
  common.rectangle.top = common.position.y;
  common.rectangle.bottom = common.position.y + SMALLCHAR_HEIGHT;
}

} // namespace ui
