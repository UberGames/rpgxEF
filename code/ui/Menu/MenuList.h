//
// Created by Walter on 08.02.2020.
//

#ifndef RPGXEF_CODE_UI_MENU_MENULIST_H_
#define RPGXEF_CODE_UI_MENU_MENULIST_H_

#include "MenuColor.h"
#include "MenuCommon.h"
#include "MenuFocus.h"
#include "MenuItem.h"
#include "MenuSize.h"

namespace ui {

struct MenuList : MenuItem {
  struct {
    std::size_t old{0};
    std::size_t current{0};
  } value;

  std::size_t item_number{0};
  std::size_t top{0};
  MenuSize size;
  std::size_t separation{0};

  MenuColor color;

  struct {
    std::size_t id{0};
    common::Point2dI position;
    std::size_t flags{0};
    MenuColor color;
  } text;
  bool updated{false};
  MenuFocus focus;
  struct {
    common::Point2dI position;
    std::size_t flags{0};
    MenuColor color;
  } list;
  std::size_t max_char_size{0};
  bool ignore_list{false};
  struct {
    std::size_t left{0};
    std::size_t right{0};
    std::size_t up{0};
    std::size_t down{0};

    struct {
      std::size_t x{0};
      std::size_t y{0};
    } offset;
  } draw_list;

  std::vector<std::string> item_names;
  std::vector<std::int32_t> list_names;
  std::vector<std::int32_t> list_shaders;

  void init() override;
};

} // namespace ui

#endif // RPGXEF_CODE_UI_MENU_MENULIST_H_
