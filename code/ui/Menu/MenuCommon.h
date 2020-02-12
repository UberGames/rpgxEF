//
// Created by Walter on 07.02.2020.
//

#ifndef RPGXEF_CODE_UI_MENU_MENUCOMMON_H_
#define RPGXEF_CODE_UI_MENU_MENUCOMMON_H_

#include <cinttypes>
#include <common/Point2d.h>
#include <common/Rectangle.h>
#include <functional>
#include <memory>

namespace ui {

class MenuFramework;

enum class MenuCommonType {
  Slider,
  Action,
  SpinControl,
  Field,
  RadioButton,
  Bitmap,
  Text,
  ScrollText,
  ScrollList,
  PText,
  BText
};

struct MenuCommon {
  MenuCommonType type;
  std::int32_t id{0};
  common::Point2dI position{};
  common::RectangleI rectangle{};
  std::string name;
  std::int32_t menu_position{0};
  std::uint64_t flags;
  MenuFramework *parent{nullptr};

  std::function<void(std::int32_t)> callback;
  std::function<void()> statusbar;
  std::function<void()> ownerdraw;
};

} // namespace ui

#endif // RPGXEF_CODE_UI_MENU_MENUCOMMON_H_
