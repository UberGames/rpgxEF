//
// Created by Walter on 07.02.2020.
//

#include "MenuCommon.h"
#include "MenuFramework.h"
#include <common/BitTools.h>
#include <ui/ui_local.h>

namespace ui {

bool MenuCommon::has_flags(std::uint64_t required_flags) const {
  return common::bit_in(flags, required_flags);
}

bool MenuCommon::blink() const { return has_flags(QMF_BLINK); }

bool MenuCommon::small_font() const { return has_flags(QMF_SMALLFONT); }

bool MenuCommon::center_justify() const {
  return has_flags(QMF_CENTER_JUSTIFY);
}

bool MenuCommon::left_justify() const { return has_flags(QMF_LEFT_JUSTIFY); }
bool MenuCommon::right_justify() const { return has_flags(QMF_RIGHT_JUSTIFY); }

bool MenuCommon::numbers_only() const { return has_flags(QMF_NUMBERSONLY); }

bool MenuCommon::highlight() const { return has_flags(QMF_HIGHLIGHT); }

bool MenuCommon::highlight_if_focus() const {
  return has_flags(QMF_HIGHLIGHT_IF_FOCUS);
}

bool MenuCommon::pulse_if_focus() const { return has_flags(QMF_PULSEIFFOCUS); }

bool MenuCommon::has_mouse_focus() const {
  return has_flags(QMF_HASMOUSEFOCUS);
}

bool MenuCommon::no_on_off_text() const { return has_flags(QMF_NOONOFFTEXT); }

bool MenuCommon::mouse_only() const { return has_flags(QMF_MOUSEONLY); }

bool MenuCommon::hidden() const { return has_flags(QMF_HIDDEN); }

bool MenuCommon::grayed() const { return has_flags(QMF_GRAYED); }

bool MenuCommon::inactive() const { return has_flags(QMF_INACTIVE); }

bool MenuCommon::no_default_init() const {
  return has_flags(QMF_NODEFAULTINIT);
}

bool MenuCommon::owner_draw() const { return has_flags(QMF_OWNERDRAW); }

bool MenuCommon::pulse() const { return has_flags(QMF_PULSE); }

bool MenuCommon::lower_case() const { return has_flags(QMF_LOWERCASE); }

bool MenuCommon::upper_case() const { return has_flags(QMF_UPPERCASE); }

bool MenuCommon::silent() const { return has_flags(QMF_SILENT); }

bool MenuCommon::alternate() const { return has_flags(QMF_ALTERNATE); }

bool MenuCommon::alternate_2() const { return has_flags(QMF_ALTERNATE2); }

[[maybe_unused]] void MenuCommon::toggle_blink() { flags ^= QMF_BLINK; }

void MenuCommon::toggle_small_fond() { flags ^= QMF_SMALLFONT; }

void MenuCommon::enable_blink() { flags |= QMF_BLINK; }

void MenuCommon::disable_blink() { flags &= ~QMF_BLINK; }

void MenuCommon::enable_small_font() { flags |= QMF_SMALLFONT; }

void MenuCommon::disable_small_font() { flags &= ~QMF_SMALLFONT; }

void MenuCommon::toggle_center_justify() {
  flags &= ~QMF_RIGHT_JUSTIFY;
  flags &= ~QMF_LEFT_JUSTIFY;
  flags ^= QMF_CENTER_JUSTIFY;
}

void MenuCommon::enable_center_justify() {
  flags &= ~QMF_LEFT_JUSTIFY;
  flags &= ~QMF_RIGHT_JUSTIFY;
  flags |= QMF_CENTER_JUSTIFY;
}

void MenuCommon::disable_center_justify() { flags &= ~QMF_CENTER_JUSTIFY; }

void MenuCommon::toggle_left_justify() {
  flags &= ~QMF_CENTER_JUSTIFY;
  flags &= ~QMF_RIGHT_JUSTIFY;
  flags ^= QMF_LEFT_JUSTIFY;
}

void MenuCommon::enable_left_justify() {
  flags &= ~QMF_RIGHT_JUSTIFY;
  flags &= ~QMF_CENTER_JUSTIFY;
  flags |= QMF_CENTER_JUSTIFY;
}

void MenuCommon::disable_left_justify() { flags &= ~QMF_CENTER_JUSTIFY; }

void MenuCommon::toggle_right_justify() {
  flags &= ~QMF_LEFT_JUSTIFY;
  flags &= QMF_CENTER_JUSTIFY;
  flags ^= QMF_RIGHT_JUSTIFY;
}

void MenuCommon::enable_right_justify() {
  flags &= ~QMF_LEFT_JUSTIFY;
  flags &= ~QMF_CENTER_JUSTIFY;
  flags |= QMF_RIGHT_JUSTIFY;
}

void MenuCommon::disable_right_justify() { flags &= ~QMF_RIGHT_JUSTIFY; }

void MenuCommon::toggle_numbers_only() { flags ^= QMF_MOUSEONLY; }

void MenuCommon::enable_numbers_only() { flags |= QMF_NUMBERSONLY; }

void MenuCommon::disable_numbers_only() { flags &= ~QMF_NUMBERSONLY; }

void MenuCommon::toggle_highlight() { flags ^= QMF_HIGHLIGHT; }

void MenuCommon::enable_highlight() { flags |= QMF_HIGHLIGHT; }

void MenuCommon::disable_highlight() { flags &= ~QMF_HIGHLIGHT; }

void MenuCommon::toogle_highlight_if_focus() {
  flags ^= QMF_HIGHLIGHT_IF_FOCUS;
}

void MenuCommon::enable_highlight_if_focus() {
  flags |= QMF_HIGHLIGHT_IF_FOCUS;
}

void MenuCommon::disable_highlight_if_focus() {
  flags &= ~QMF_HIGHLIGHT_IF_FOCUS;
}

void MenuCommon::toggle_pulse_if_focus() { flags ^= QMF_PULSEIFFOCUS; }

void MenuCommon::enable_pulse_if_focus() { flags |= QMF_PULSEIFFOCUS; }

void MenuCommon::disable_pulse_if_focus() { flags &= ~QMF_PULSEIFFOCUS; }

void MenuCommon::toggle_has_mouse_focus() { flags ^= QMF_HASMOUSEFOCUS; }

void MenuCommon::enable_has_mouse_focus() { flags |= QMF_HASMOUSEFOCUS; }

void MenuCommon::disable_has_mouse_focus() { flags &= ~QMF_HASMOUSEFOCUS; }

void MenuCommon::toggle_no_on_off_text() { flags ^= QMF_NOONOFFTEXT; }

void MenuCommon::enable_no_on_off_text() { flags |= QMF_NOONOFFTEXT; }

void MenuCommon::disable_no_on_off_text() { flags &= ~QMF_NOONOFFTEXT; }

void MenuCommon::toggle_mouse_only() { flags ^= QMF_MOUSEONLY; }

void MenuCommon::enable_mouse_only() { flags |= QMF_MOUSEONLY; }

void MenuCommon::disable_mouse_only() { flags &= ~QMF_MOUSEONLY; }

void MenuCommon::hide() { flags |= QMF_HIDDEN; }

void MenuCommon::show() { flags &= ~QMF_HIDDEN; }

void MenuCommon::toggle_grayed() { flags ^= QMF_GRAYED; }

void MenuCommon::enable_grayed() { flags |= QMF_GRAYED; }

void MenuCommon::disable_grayed() { flags &= ~QMF_GRAYED; }

void MenuCommon::activate() { flags &= ~QMF_INACTIVE; }

void MenuCommon::deactivate() { flags |= QMF_INACTIVE; }

void MenuCommon::enable_no_default_init() { flags |= QMF_NODEFAULTINIT; }

void MenuCommon::disable_no_default_init() { flags &= ~QMF_NODEFAULTINIT; }

void MenuCommon::enable_owner_draw() { flags |= QMF_OWNERDRAW; }

void MenuCommon::disable_owner_draw() { flags &= ~QMF_OWNERDRAW; }

void MenuCommon::toggle_pulse() { flags ^= QMF_PULSE; }

void MenuCommon::enable_pulse() { flags |= QMF_PULSE; }

void MenuCommon::disable_pulse() { flags &= ~QMF_PULSE; }

void MenuCommon::toggle_lower_case() {
  if (lower_case()) {
    disable_lower_case();
  } else {
    disable_upper_case();
    enable_lower_case();
  }
}

void MenuCommon::enable_lower_case() {
  flags &= ~QMF_UPPERCASE;
  flags |= QMF_LOWERCASE;
}

void MenuCommon::disable_lower_case() { flags &= ~QMF_LOWERCASE; }

void MenuCommon::toggle_upper_case() {
  if (upper_case()) {
    disable_upper_case();
  } else {
    disable_lower_case();
    enable_upper_case();
  }
}

void MenuCommon::enable_upper_case() {
  flags &= ~QMF_LOWERCASE;
  flags |= QMF_UPPERCASE;
}

void MenuCommon::disable_upper_case() { flags &= ~QMF_UPPERCASE; }

void MenuCommon::toggle_silent() { flags ^= QMF_SILENT; }

void MenuCommon::enable_silent() { flags |= QMF_SILENT; }

void MenuCommon::disable_silent() { flags &= ~QMF_SILENT; }

void MenuCommon::toogle_alternate() { flags ^= QMF_ALTERNATE; }

void MenuCommon::enable_alternate() { flags |= QMF_ALTERNATE; }

void MenuCommon::disable_alternate() { flags &= ~QMF_ALTERNATE; }

void MenuCommon::toogle_alternate_2() { flags ^= QMF_ALTERNATE2; }

void MenuCommon::enable_alternate_2() { flags |= QMF_ALTERNATE2; }

void MenuCommon::disable_alternate_2() { flags &= ~QMF_ALTERNATE2; }

bool MenuCommon::has_focus() const {
  return parent->getCursor() == menu_position;
}

bool MenuCommon::is_current_menu() const {
  const auto mf_cursor = parent->getCursor();
  if (mf_cursor < 0) {
    return false;
  }

  return mf_cursor == menu_position;
}

} // namespace ui
