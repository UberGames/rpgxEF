//
// Created by Walter on 07.02.2020.
//
#include "MenuFramework.h"
#include "MenuItem.h"
#include <ui/ui_local.h>

namespace ui {

void MenuFramework::setCursor(std::int32_t new_cursor) {
  cursor.previous = cursor.current;
  cursor.current = new_cursor;
}

void MenuFramework::addItem(const std::shared_ptr<MenuItem> &item) {
  items.emplace_back(item);
  item->common.parent = this;
  item->common.menu_position = items.size() - 1;
  item->common.disable_has_mouse_focus();

  if (!item->common.no_default_init()) {
    item->init();
  }
}

} // namespace ui
