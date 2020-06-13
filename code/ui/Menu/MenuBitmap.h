//
// Created by Walter on 08.02.2020.
//

#ifndef RPGXEF_CODE_UI_MENU_MENUBITMAP_H_
#define RPGXEF_CODE_UI_MENU_MENUBITMAP_H_

#include "MenuItem.h"
#include "MenuShader.h"
#include "MenuFocus.h"
#include "MenuColor.h"

namespace ui {

struct MenuBitmap : MenuItem {
  MenuShader shader;
  MenuShader shader_focus;
  MenuShader shader_error;
  MenuFocus focus;
  MenuSize size;
  MenuColor color;
  struct MenuBitmapText {
    std::size_t id{0};
    std::size_t id2{0};
    common::Point2dI position;
    MenuColor color;
    std::size_t style{0};
    std::string text;
    std::string text_2;

    [[nodiscard]] std::string_view get_text() const;
    [[nodiscard]] std::string_view get_text_2() const;
    operator bool() const;
  } text;

  void init() override;
  void draw() override;
};

} // namespace ui

#endif // RPGXEF_CODE_UI_MENU_MENUBITMAP_H_
