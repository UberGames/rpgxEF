//
// Created by Walter on 08.02.2020.
//

#include "MenuSlider.h"
#include "MenuFramework.h"
#include <common/ColorTable.h>
#include <ui/Atoms.h>
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

void MenuSlider::draw() {
  auto text_color = common::Color::Red;
  auto thumb_color = common::Color::LightGrey;
  auto background_color = common::Color::DarkGrey;
  auto c = common::Color{};

  if (common.grayed()) {
    c = common::Color::DarkGrey;
  } else if (common.is_current_menu()) {
    c = color.highlight;
    text_color = text.color.highlight;
    thumb_color = thumb.color.highlight;
    background_color = common::Color::LightGrey;
  } else {
    c = color.normal;
    text_color = text.color.normal;
    thumb_color = thumb.color.normal;
  }

  if (trap_Key_IsDown(K_MOUSE1) && mouse_down) {
    mouse_down = false;
    common.parent->disableNoSelecting();
  }

  if (mouse_down) {
    value.current = (static_cast<float>(uis.cursorx - common.position.x) /
                     static_cast<float>(focus.size.width)) *
                        (value.max - value.min) +
                    value.min;
    Com_Clamp(value.min, value.max, value.current);
    common.parent->enableNoSelecting();
  }

  if (picture.shader) {
    trap_R_SetColor(c.values);
    Atoms::DrawHandlePic({picture.position.x, picture.position.y,
                          picture.size.width, picture.size.height},
                         picture.shader);
    trap_R_SetColor(nullptr);
  }

  if (text.id) {
    Atoms::DrawProportionalString({picture.position.x + text.position.x,
                                   picture.position.y + text.position.y},
                                  menu_button_text[text.id][0], UI_SMALLFONT,
                                  text_color);
  }

  if (common.has_focus() && menu_button_text[text.id][1]) {
    Atoms::DrawProportionalString(common.parent->getDescriptionPosition(),
                                  menu_button_text[text.id][1],
                                  UI_LEFT | UI_TINYFONT, common::Color::Black);
  }

  if (value.max > value.min) {
    range = (value.current - value.min) / (value.max - value.min);
    if (range < 0) {
      range = 0;
    } else if (range > 1) {
      range = 1;
    }
  } else {
    range = 0;
  }

  const auto thumb_x = common.position.x - (thumb.graphic_width / 2.0) +
                       (focus.size.width * range);
  if (range > 0.0) {
    trap_R_SetColor(background_color.values);
    Atoms::DrawHandlePic({common.position.x, common.position.y,
                          (thumb_x - common.position.x) + 4, 18},
                         uis.whiteShader);
  }

  if (graph.shader) {
    trap_R_SetColor(c.values);
    Atoms::DrawHandlePic({common.position.x, common.position.y,
                          graph.size.width, graph.size.height},
                         graph.shader);
  }

  if (thumb.shader) {
    trap_R_SetColor(thumb_color.values);
    Atoms::DrawHandlePic(
        {thumb_x, common.position.x, thumb.size.width, thumb.size.height},
        thumb.shader);
  }
}

} // namespace ui
