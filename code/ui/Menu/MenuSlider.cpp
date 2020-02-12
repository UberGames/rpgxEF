//
// Created by Walter on 08.02.2020.
//

#include "MenuSlider.h"
#include <ui/ui_local.h>

namespace ui {

void MenuSlider::init() {
  if (picture.position.x) {
    common.rectangle.left = picture.position.x;
    common.rectangle.right = picture.position.x + focus.size.width;
    common.rectangle.top = picture.position.y;
    common.rectangle.bottom = picture.position.y + focus.size.height;
  } else {
    common.rectangle.left = common.position.x;
    common.rectangle.right = common.position.x + focus.size.width;
    common.rectangle.top = common.position.y;
    common.rectangle.bottom = common.position.y + focus.size.height;
  }

  if (!thumb.name.empty()) {
    thumb.shader = trap_R_RegisterShaderNoMip(thumb.name.c_str());
  }

  if (!picture.name.empty()) {
    picture.shader = trap_R_RegisterShaderNoMip(picture.name.c_str());
  }

  if (!common.name.empty()) {
    graph.shader = trap_R_RegisterShaderNoMip(common.name.c_str());
  }
}

} // namespace ui
