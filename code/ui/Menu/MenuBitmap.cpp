//
// Created by Walter on 08.02.2020.
//

#include "MenuBitmap.h"
#include "MenuFramework.h"
#include <common/BitTools.h>
#include <ui/Atoms.h>
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

  if (h < 0) {
    h = -h;
  }

  if (common.right_justify()) {
    x = x - w;
  } else if (common.center_justify()) {
    x = x - w / 2;
  }

  common.rectangle.left = x;
  common.rectangle.right = x + w;
  common.rectangle.top = y;
  common.rectangle.bottom = y + h;

  shader.handle = 0;
  shader_focus.handle = 0;
}

void MenuBitmap::draw() {
  auto text_style = text.style;
  auto x = common.position.x;
  auto y = common.position.y;
  const auto w = size.width;
  const auto h = size.height;
  auto highlight = false;

  if (common.right_justify()) {
    x = x - w;
  } else if (common.center_justify()) {
    x = x - w / 2;
  }

  if (common.grayed()) {
    if (shader) {
      trap_R_SetColor(common::Color::MediumGrey.values);
      Atoms::DrawHandlePic({x, y, w, h}, shader.handle);
      trap_R_SetColor(nullptr);
    }
  } else if ((common.highlight_if_focus() && common.has_focus()) ||
             common.highlight()) {
    trap_R_SetColor(color.highlight.values);
    highlight = true;

    if (shader) {
      Atoms::DrawHandlePic({x, y, w, h}, shader.handle);
    }

    if (menu_button_text[text.id][1]) {
      Atoms::DrawProportionalString(
          common.parent->getDescriptionPosition(), menu_button_text[text.id][1],
          UI_LEFT | UI_TINYFONT, common::Color::Black);
    }

    trap_R_SetColor(nullptr);
  } else {
    if (shader) {
      trap_R_SetColor(color.normal.values);
      Atoms::DrawHandlePic({x, y, w, h}, shader.handle);
    }

    if (common.pulse() || (common.pulse_if_focus() && common.has_focus())) {
      auto c = color.highlight;
      c.a = 0.5 * 0.5 * std::sin(uis.realtime / PULSE_DIVISOR);
      trap_R_SetColor(c.values);
      Atoms::DrawHandlePic({x, y, w, h}, shader_focus.handle);
      trap_R_SetColor(nullptr);
    } else if (common.highlight() ||
               (common.highlight_if_focus() && common.has_focus())) {
      trap_R_SetColor(color.highlight.values);
      Atoms::DrawHandlePic({x, y, w, h}, shader_focus.handle);
      trap_R_SetColor(nullptr);
    }
  }

  if (text) {
    auto color_i = common::Color{};
    if (common.highlight()) {
      color_i = text.color.highlight;
    } else {
      color_i = text.color.normal;
    }

    Atoms::DrawProportionalString({x + text.position.x, y + text.position.y},
                                  text.get_text(), text_style, color_i);

    const auto text_2 = text.get_text_2();
    if (!text_2.empty()) {
      auto inc_y = 0;
      if (common::bit_in(text_style, UI_SMALLFONT)) {
        inc_y = PROP_HEIGHT * 1.15;
      } else if (common::bit_in(text_style, UI_TINYFONT)) {
        inc_y = PROP_TINY_HEIGHT * 1.15;
      } else {
        inc_y = PROP_HEIGHT * 1.15;
      }

      Atoms::DrawProportionalString(
          {x + text.position.x, y + text.position.y + inc_y}, text_2,
          text_style, color_i);
    }
  }
}

MenuBitmap::MenuBitmapText::operator bool() const {
  return id > 0 || !text.empty();
}

std::string_view MenuBitmap::MenuBitmapText::get_text() const {
  if (id > 0 && menu_button_text[id][0]) {
    return menu_button_text[id][0];
  }

  return text;
}

std::string_view MenuBitmap::MenuBitmapText::get_text_2() const {
  if (id2 > 0 && menu_button_text[id2][0]) {
    return menu_button_text[id2][0];
  }

  return text_2;
}

} // namespace ui
