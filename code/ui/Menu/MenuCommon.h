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

  [[nodiscard]] [[maybe_unused]] bool
  has_flags(std::uint64_t required_flags) const;
  [[nodiscard]] [[maybe_unused]] bool blink() const;
  [[maybe_unused]] void toggle_blink();
  [[maybe_unused]] void enable_blink();
  [[maybe_unused]] void disable_blink();
  [[nodiscard]] [[maybe_unused]] bool small_font() const;
  [[maybe_unused]] void toggle_small_fond();
  [[maybe_unused]] void enable_small_font();
  [[maybe_unused]] void disable_small_font();
  [[nodiscard]] [[maybe_unused]] bool center_justify() const;
  [[maybe_unused]] void toggle_center_justify();
  [[maybe_unused]] void enable_center_justify();
  [[maybe_unused]] void disable_center_justify();
  [[nodiscard]] [[maybe_unused]] bool left_justify() const;
  [[maybe_unused]] void toggle_left_justify();
  [[maybe_unused]] void enable_left_justify();
  [[maybe_unused]] void disable_left_justify();
  [[nodiscard]] [[maybe_unused]] bool right_justify() const;
  [[maybe_unused]] void toggle_right_justify();
  [[maybe_unused]] void enable_right_justify();
  [[maybe_unused]] void disable_right_justify();
  [[nodiscard]] [[maybe_unused]] bool numbers_only() const;
  [[maybe_unused]] void toggle_numbers_only();
  [[maybe_unused]] void enable_numbers_only();
  [[maybe_unused]] void disable_numbers_only();
  [[nodiscard]] [[maybe_unused]] bool highlight() const;
  [[maybe_unused]] void toggle_highlight();
  [[maybe_unused]] void enable_highlight();
  [[maybe_unused]] void disable_highlight();
  [[nodiscard]] [[maybe_unused]] bool highlight_if_focus() const;
  [[maybe_unused]] void toogle_highlight_if_focus();
  [[maybe_unused]] void enable_highlight_if_focus();
  [[maybe_unused]] void disable_highlight_if_focus();
  [[nodiscard]] [[maybe_unused]] bool pulse_if_focus() const;
  [[maybe_unused]] void toggle_pulse_if_focus();
  [[maybe_unused]] void enable_pulse_if_focus();
  [[maybe_unused]] void disable_pulse_if_focus();
  [[nodiscard]] [[maybe_unused]] bool has_mouse_focus() const;
  [[maybe_unused]] void toggle_has_mouse_focus();
  [[maybe_unused]] void enable_has_mouse_focus();
  [[maybe_unused]] void disable_has_mouse_focus();
  [[nodiscard]] [[maybe_unused]] bool no_on_off_text() const;
  [[maybe_unused]] void toggle_no_on_off_text();
  [[maybe_unused]] void enable_no_on_off_text();
  [[maybe_unused]] void disable_no_on_off_text();
  [[nodiscard]] [[maybe_unused]] bool mouse_only() const;
  [[maybe_unused]] void toggle_mouse_only();
  [[maybe_unused]] void enable_mouse_only();
  [[maybe_unused]] void disable_mouse_only();
  [[nodiscard]] [[maybe_unused]] bool hidden() const;
  [[maybe_unused]] void hide();
  [[maybe_unused]] void show();
  [[nodiscard]] [[maybe_unused]] bool grayed() const;
  [[maybe_unused]] void toggle_grayed();
  [[maybe_unused]] void enable_grayed();
  [[maybe_unused]] void disable_grayed();
  [[nodiscard]] [[maybe_unused]] bool inactive() const;
  [[maybe_unused]] void activate();
  [[maybe_unused]] void deactivate();
  [[nodiscard]] [[maybe_unused]] bool no_default_init() const;
  [[maybe_unused]] void enable_no_default_init();
  [[maybe_unused]] void disable_no_default_init();
  [[nodiscard]] [[maybe_unused]] bool owner_draw() const;
  [[maybe_unused]] void enable_owner_draw();
  [[maybe_unused]] void disable_owner_draw();
  [[nodiscard]] [[maybe_unused]] bool pulse() const;
  [[maybe_unused]] void toggle_pulse();
  [[maybe_unused]] void enable_pulse();
  [[maybe_unused]] void disable_pulse();
  [[nodiscard]] [[maybe_unused]] bool lower_case() const;
  [[maybe_unused]] void toggle_lower_case();
  [[maybe_unused]] void enable_lower_case();
  [[maybe_unused]] void disable_lower_case();
  [[nodiscard]] [[maybe_unused]] bool upper_case() const;
  [[maybe_unused]] void toggle_upper_case();
  [[maybe_unused]] void enable_upper_case();
  [[maybe_unused]] void disable_upper_case();
  [[nodiscard]] [[maybe_unused]] bool silent() const;
  [[maybe_unused]] void toggle_silent();
  [[maybe_unused]] void enable_silent();
  [[maybe_unused]] void disable_silent();
  [[nodiscard]] [[maybe_unused]] bool alternate() const;
  [[maybe_unused]] void toogle_alternate();
  [[maybe_unused]] void enable_alternate();
  [[maybe_unused]] void disable_alternate();
  [[nodiscard]] [[maybe_unused]] bool alternate_2() const;
  [[maybe_unused]] void toogle_alternate_2();
  [[maybe_unused]] void enable_alternate_2();
  [[maybe_unused]] void disable_alternate_2();

  [[nodiscard]] [[maybe_unused]] bool has_focus() const;
  [[nodiscard]] [[maybe_unused]] bool is_current_menu() const;
};

} // namespace ui

#endif // RPGXEF_CODE_UI_MENU_MENUCOMMON_H_
