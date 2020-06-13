//
// Created by Walter on 08.02.2020.
//

#include "MenuField.h"
#include "../ui_local.h"
#include "MenuColors.h"
#include "MenuFieldData.h"
#include "MenuFramework.h"
#include <common/BitTools.h>
#include <ui/Atoms.h>

namespace ui {

void MenuField::init() {
  auto l = 0;
  auto w = 0;
  auto h = 0;

  clear();

  if (common::bit_in(data.style, UI_TINYFONT)) {
    w = TINYCHAR_WIDTH;
    h = TINYCHAR_HEIGHT;
  } else if (common::bit_in(data.style, UI_BIGFONT)) {
    w = BIGCHAR_WIDTH;
    h = BIGCHAR_HEIGHT;
  } else if (common::bit_in(data.style, UI_GIANTFONT)) {
    w = GIANTCHAR_WIDTH;
    h = GIANTCHAR_HEIGHT;
  } else {
    w = SMALLCHAR_WIDTH;
    h = SMALLCHAR_HEIGHT;
  }

  l = 0;

  if (common::bit_in(data.style, UI_CENTER)) {
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

void MenuField::draw() {
  const auto x = common.position.x;
  const auto y = common.position.y;
  auto offset = common.name.empty() ? 0 : MENU_BUTTON_MED_HEIGHT + 4;
  auto w = 0;
  auto style = data.style;

  if (common::bit_in(style, UI_TINYFONT)) {
    w = TINYCHAR_HEIGHT;
  } else if (common::bit_in(style, UI_BIGFONT)) {
    w = BIGCHAR_WIDTH;
  } else if (common::bit_in(style, UI_GIANTFONT)) {
    w = GIANTCHAR_WIDTH;
  } else {
    w = SMALLCHAR_WIDTH;
  }

  const auto focus = common.has_focus();
  if (focus) {
    style |= UI_SHOWCOLOR;
  }

  if (!common.name.empty() && common::bit_in(style, UI_CENTER)) {
    const auto width =
        (((MENU_BUTTON_MED_HEIGHT * 2) - 16) + common.name.length() * w) +
        2 * w;

    if (focus) {
      trap_R_SetColor(common::Color::LightPurple.values);
    } else if (common.grayed()) {
      trap_R_SetColor(common::Color::MediumGrey.values);
    } else {
      trap_R_SetColor(common::Color::DarkPurple.values);
    }

    Atoms::DrawHandlePic({x - (width >> 1), y, 19, 19},
                         uis.graphicButtonLeftEnd);
    Atoms::DrawHandlePic({(x + (width >> 1)) - 19, y, -19, 19},
                         uis.graphicButtonLeftEnd);
    Atoms::DrawHandlePic(
        {(x - (width >> 1)) + (19 - 8), y, width - ((19 * 2) - 16), 19},
        uis.whiteShader);

    auto color = focus ? common::Color::White : common::Color::Black;
    Atoms::DrawProportionalString({x, y + 2}, common.name,
                                  UI_CENTER | UI_SMALLFONT, color);
  }

  auto color = common::Color{};
  if (common.grayed()) {
    color = common::Color::DarkGrey;
  } else if (focus) {
    color = data.text.color.highlight;
  } else {
    color = data.text.color.normal;
  }

  if (focus) {
    Atoms::FillRect({common.rectangle.left, common.rectangle.top + offset,
                     common.rectangle.width() + 1,
                     (common.rectangle.height() + 1) - offset},
                    ui::colors::listbar_color);

    if (data.title.id && menu_button_text[data.title.id][1]) {
      Atoms::DrawProportionalString(common.parent->getDescriptionPosition(),
                                    menu_button_text[data.title.id][1],
                                    UI_LEFT | UI_TINYFONT,
                                    common::Color::Black);
    }
  }

  if (data.title.id) {
    auto title_color = data.title.color.normal;

    if (common.grayed()) {
      title_color = common::Color::DarkGrey;
    }

    Atoms::DrawProportionalString({x - 5, y},
                                  menu_button_text[data.title.id][0],
                                  UI_RIGHT | UI_SMALLFONT, title_color);
  }

  // TODO draw menu field (MField_Draw)
}

} // namespace ui
