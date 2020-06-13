//
// Created by Walter on 08.02.2020.
//

#include "MenuAction.h"
#include "MenuFramework.h"
#include <common/BitTools.h>
#include <ui/Atoms.h>

namespace ui {

void MenuAction::init() {
  auto x = common.position.x;
  auto y = common.position.y;
  auto w = size.width;
  auto h = size.height;

  if (common.right_justify()) {
    x = x - w;
  } else if (common.center_justify()) {
    x = x - w / 2;
  }

  common.rectangle.left = x;
  common.rectangle.right = x + w;
  common.rectangle.top = y;
  common.rectangle.bottom = y + h;
}

void MenuAction::draw() {
  auto button_color = common::Color{};
  auto text_color = common::Color{};
  auto style = 0;
  if (common.grayed()) {
    button_color = color.greyed;
    text_color = text.color.greyed;
  } else if (common.pulse_if_focus() && common.has_focus()) {
    button_color = color.highlight;
    text_color = text.color.highlight;
    style = UI_PULSE;
  } else if (common.highlight_if_focus() && common.has_focus()) {
    button_color = color.highlight;
    text_color = text.color.highlight;
  } else if (common.blink()) {
    if ((uis.realtime / BLINK_DIVISOR) & 1) {
      button_color = color.normal;
      text_color = text.color.normal;
    } else {
      button_color = color.highlight;
      text_color = color.highlight;
    }
    style = UI_BLINK;
  } else {
    button_color = color.normal;
    text_color = text.color.normal;
  }

  const auto x = common.position.x;
  const auto y = common.position.y;

  trap_R_SetColor(button_color.values);
  Atoms::DrawHandlePic({x, y}, uis.whiteShader);

  if (menu_button_text[text.id][0]) {
    Atoms::DrawProportionalString({x + text.position.x, y + text.position.y},
                                  menu_button_text[text.id][0],
                                  style | UI_SMALLFONT, text_color);
  }

  if (text.id2 > 0) {
    auto inc_y = common::bit_in(style, UI_SMALLFONT) ? PROP_HEIGHT * 1.15
                                                     : PROP_TINY_HEIGHT * 1.15;
    Atoms::DrawProportionalString(
        {x + text.position.x, y + text.position.y + inc_y},
        menu_button_text[text.id2][0], style, text_color);
  }
}

} // namespace ui
