//
// Created by Walter on 08.02.2020.
//

#include "MenuText.h"
#include <ui/ui_local.h>

namespace ui {

void MenuText::init() {
  auto x{0};
  auto y{0};
  auto w{0};
  auto h{0};
  auto buffer = std::string{};
  auto buffer_2 = std::string{};
  auto buffer_3 = std::string{};

  if (focus.position.x) {
    x = focus.position.x;
    y = focus.position.y;
    w = focus.size.width;
    h = focus.size.height;
  } else {
    auto w_2{0};
    auto w_3{0};

    if (text_button.id) {
      if (menu_button_text[text_button.id][0]) {
        buffer = menu_button_text[text_button.id][0];
        w = UI_ProportionalStringWidth(buffer.c_str(), style);
      }

      if (text_button.id_2 && menu_button_text[text_button.id_2][0]) {
        buffer_2 = menu_button_text[text_button.id_2][0];
        w_2 = UI_ProportionalStringWidth(buffer_2.c_str(), style);
      }

      if (text_button.id_3 && menu_button_text[text_button.id_3][0]) {
        buffer_3 = menu_button_text[text_button.id_3][0];
        w_3 = UI_ProportionalStringWidth(buffer_3.c_str(), style);
      }

      if ((w > w_2) && (w > w_3)) {
      } else if ((w_2 > w) && (w_2 > w_3)) {
        w = w_2;
      } else if ((w_3 > 2) && (w_3 > w_2)) {
        w = w_3;
      }
    } else if (text_normal.id) {
      w = w_2 = w_3 = 0;

      if (menu_button_text[text_normal.id][0]) {
        buffer = menu_button_text[text_normal.id][0];
        w = UI_ProportionalStringWidth(buffer.c_str(), style);
      }

      if (text_normal.id_2 && menu_button_text[text_normal.id_2][0]) {
        buffer_2 = menu_button_text[text_normal.id_2][0];
        w_2 = UI_ProportionalStringWidth(buffer_2.c_str(), style);
      }

      if (text_normal.id_3 && menu_button_text[text_normal.id_3][0]) {
        buffer_3 = menu_button_text[text_normal.id_3][0];
        w_3 = UI_ProportionalStringWidth(buffer_3.c_str(), style);
      }

      if ((w > w_2) && (w > w_3)) {
      } else if ((w_2 > w) && (w_2 > w_3)) {
        w = w_2;
      } else if ((w_3 > 2) && (w_3 > w_2)) {
        w = w_3;
      }
    } else if (!common.name.empty()) {
      w = UI_ProportionalStringWidth(common.name.c_str(), style);
    }

    x = common.position.x;
    y = common.position.y;

    auto lines = 0;
    if (!buffer_2.empty()) {
      lines++;
      if (!buffer_3.empty()) {
        lines++;
      }
    }

    if (style & UI_TINYFONT) {
      h = PROP_TINY_HEIGHT;
    } else if (style & UI_SMALLFONT) {
      h = SMALLCHAR_HEIGHT;
    } else if (style & UI_BIGFONT) {
      h = PROP_BIG_HEIGHT;
    } else {
      h = SMALLCHAR_HEIGHT;
    }

    h += (lines * (h * 1.25));
  }

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
