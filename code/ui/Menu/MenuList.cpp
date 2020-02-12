//
// Created by Walter on 08.02.2020.
//

#include "MenuList.h"
#include "../ui_local.h"

namespace ui {

void MenuList::init() {
  if (common.type == MenuCommonType::SpinControl) {
    if (list_names.empty() && item_names.empty()) {
      return;
    }

    // Count number of choices for this spin control
    item_number = 0;

    if (!list_names.empty()) {
      while (menu_normal_text[list_names.back()] != nullptr) {
        item_number++;
      }
    } else {
      item_number = item_names.size();
    }

    if (!focus.size.width && !focus.size.height) {
      common.rectangle.left = common.position.x;
      common.rectangle.right = common.position.x + MENU_BUTTON_MED_HEIGHT +
                               MENU_BUTTON_MED_WIDTH + MENU_BUTTON_MED_HEIGHT;
      common.rectangle.top = common.position.y;
      common.rectangle.bottom = common.position.y + MENU_BUTTON_MED_HEIGHT;
    } else {
      common.rectangle.left = common.position.x;
      common.rectangle.right = common.position.x + focus.size.width;
      common.rectangle.top = common.position.y;
      common.rectangle.bottom = common.position.y + focus.size.height;
    }
  }
}

} // namespace ui
