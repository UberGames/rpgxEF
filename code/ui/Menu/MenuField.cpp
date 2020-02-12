//
// Created by Walter on 08.02.2020.
//

#include "MenuField.h"
#include "../ui_local.h"
#include "MenuFieldData.h"

namespace ui {

void MenuField::init() {
  auto l = 0;
  auto w = 0;
  auto h = 0;

  clear();

  if (data.style & UI_TINYFONT) {
    w = TINYCHAR_WIDTH;
    h = TINYCHAR_HEIGHT;
  } else if (data.style & UI_BIGFONT) {
    w = BIGCHAR_WIDTH;
    h = BIGCHAR_HEIGHT;
  } else if (data.style & UI_GIANTFONT) {
    w = GIANTCHAR_WIDTH;
    h = GIANTCHAR_HEIGHT;
  } else {
    w = SMALLCHAR_WIDTH;
    h = SMALLCHAR_HEIGHT;
  }

  l = 0;

  if (data.style & UI_CENTER) {
    common.rectangle.right = common.position.x + (w + (data.width * w)) / 2;
    common.rectangle.bottom = common.position.y + h;
    common.rectangle.left =
        common.position.x - ((common.rectangle.right - common.position.x));
    common.rectangle.top = common.position.y;
  } else {
    common.rectangle.left = common.position.x - l;
    common.rectangle.top = common.position.y;
    common.rectangle.right = common.position.x + w + data.width * w;
    common.rectangle.bottom = common.position.y + h;
  }

  if (!common.name.empty()) {
    common.rectangle.bottom += MENU_BUTTON_MED_HEIGHT + 4;
  }
}

void MenuField::clear() {
  data.text.buffer.clear();
  data.cursor = 0;
  data.scroll = 0;
}

} // namespace ui
