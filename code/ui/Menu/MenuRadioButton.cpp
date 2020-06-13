//
// Created by Walter on 08.02.2020.
//

#include "MenuRadioButton.h"
#include "MenuColors.h"
#include <base_game/q_shared.h>
#include <common/Color.h>
#include <ui/Atoms.h>

namespace ui {

void MenuRadioButton::init() {
  auto length = common.name.empty() ? 0 : common.name.length();
  common.rectangle.left = common.position.x - (length + 1) * SMALLCHAR_WIDTH;
  common.rectangle.right = common.position.x + 6 * SMALLCHAR_WIDTH;
  common.rectangle.top = common.position.y;
  common.rectangle.bottom = common.position.y + SMALLCHAR_HEIGHT;
}

void MenuRadioButton::draw() {
  const auto x = common.position.x;
  const auto y = common.position.y;
  auto style = 0;
  auto color = common::Color{};

  if (common.grayed()) {
    color = colors::text_disabled;
    style = UI_LEFT | UI_SMALLFONT;
  } else if (common.has_focus()) {
    color = colors::text_highlight;
    style = UI_LEFT | UI_PULSE | UI_SMALLFONT;
  } else {
    color = colors::text_normal;
    style = UI_LEFT | UI_SMALLFONT;
  }

  if (common.has_focus()) {
    Atoms::FillRect({common.rectangle.left, common.rectangle.top,
                     common.rectangle.right - common.rectangle.left + 1,
                     common.rectangle.bottom - common.rectangle.top + 1},
                    colors::listbar_color);
    Atoms::DrawChar({x, y}, 13, UI_CENTER | UI_BLINK | UI_SMALLFONT, color);
  }

  if (!common.name.empty()) {
    Atoms::DrawString({x - SMALLCHAR_WIDTH, y}, common.name,
                      UI_RIGHT | UI_SMALLFONT, color, true);
  }

  if (value == 0) {
    Atoms::DrawHandlePic({x + SMALLCHAR_WIDTH, y + 2, 16, 16}, uis.rb_off);
    Atoms::DrawString({x + SMALLCHAR_WIDTH + 16, y}, "off", style, color, true);
  } else {
    Atoms::DrawHandlePic({x + SMALLCHAR_WIDTH, y + 2, 16, 16}, uis.rb_on);
    Atoms::DrawString({x + SMALLCHAR_WIDTH + 16, y}, "on", style, color, true);
  }
}

} // namespace ui
