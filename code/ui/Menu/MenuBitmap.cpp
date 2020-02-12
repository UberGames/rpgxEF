//
// Created by Walter on 08.02.2020.
//

#include "MenuBitmap.h"
#include <ui/ui_local.h>

namespace ui {

void MenuBitmap::init() {
  auto x = common.position.x;
  auto y = common.position.y;
  auto w = size.width;
  auto h = size.height;

  if (w < 0) {
    w = -w;
  }

  if(h < 0) {
    h = -h;
  }

  if(common.flags & QMF_RIGHT_JUSTIFY) {
    x = x - w;
  } else if(common.flags & QMF_CENTER_JUSTIFY) {
    x = x - w / 2;
  }

  common.rectangle.left = x;
  common.rectangle.right = x + w;
  common.rectangle.top = y;
  common.rectangle.bottom = y + h;

  shader.handle = 0;
  shader_focus.handle = 0;
}

} // namespace ui
