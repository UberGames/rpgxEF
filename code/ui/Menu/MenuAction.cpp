//
// Created by Walter on 08.02.2020.
//

#include "MenuAction.h"
#include "../ui_local.h"

namespace ui {

void MenuAction::init() {
  auto x = common.position.x;
  auto y = common.position.y;
  auto w = size.width;
  auto h = size.height;

  if (common.flags & QMF_RIGHT_JUSTIFY) {
    x = x - w;
  } else if (common.flags & QMF_CENTER_JUSTIFY) {
    x = x - w / 2;
  }

  common.rectangle.left = x;
  common.rectangle.right = x + w;
  common.rectangle.top = y;
  common.rectangle.bottom = y + h;
}

} // namespace ui
