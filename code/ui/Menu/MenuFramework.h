//
// Created by Walter on 07.02.2020.
//

#ifndef RPGXEF_CODE_UI_MENU_MENUFRAMEWORK_H_
#define RPGXEF_CODE_UI_MENU_MENUFRAMEWORK_H_

#include "../../base_game/q_shared.h"
#include "../../common/Point2d.h"
#include <cstdint>
#include <functional>
#include <memory>
#include <vector>

namespace ui {

struct MenuItem;

class MenuFramework {
  struct Cursor {
    std::int32_t current = 0;
    std::int32_t previous = 0;
  };

  Cursor cursor;
  std::vector<std::shared_ptr<MenuItem>> items;
  bool wrap_around = false;
  bool fullscreen = false;
  bool no_background = false;
  bool initialized = false;
  bool no_selecting = false;
  common::Point2dI description_position{};
  common::Point2dI list_position{};
  common::Point2dI title_position{};
  std::int32_t title = 0;
  std::int32_t foot_note = 0;

  // TODO: add missing spin list

public:
  void toggleWrapAround() { wrap_around = !wrap_around; }
  void enableWrapAround() { wrap_around = true; }
  void disableWrapAround() { wrap_around = false; }
  [[nodiscard]] bool isWrapAround() const { return wrap_around; }

  void toggleFullscreen() { fullscreen = !fullscreen; }
  void enableFullscreen() { fullscreen = true; }
  void disableFullscreen() { fullscreen = false; }
  [[nodiscard]] bool isFullscreen() const { return fullscreen; }

  void toggleNoBackground() { no_background = !no_background; }
  void enableNoBackground() { no_background = true; }
  void disableNoBackground() { no_background = true; }
  [[nodiscard]] bool isNoBackground() const { return no_background; }

  [[nodiscard]] bool isInitialized() const { return initialized; }

  void toggleNoSelecting() { no_selecting = !no_selecting; }
  void enableNoSelecting() { no_selecting = true; }
  void disableNoSelecting() { no_selecting = false; }
  [[nodiscard]] bool isNoSelecting() const { return no_selecting; }

  [[nodiscard]] std::int32_t getCursor() const { return cursor.current; }
  void setCursor(std::int32_t new_cursor);

  [[nodiscard]] const std::vector<std::shared_ptr<MenuItem>> &
  getMenuItems() const {
    return items;
  }

  void setTitle(std::int32_t _title) { title = _title; }
  void resetTitle() { title = 0; }

  void setFootNote(std::int32_t _foot_note) { foot_note = _foot_note; }
  void resetFootNote() { foot_note = 0; }

  void setTitlePosition(const common::Point2dI &position) {
    title_position = position;
  }
  void resetTitlePosition() { title_position = {}; }

  void setDescriptionPosition(const common::Point2dI &position) {
    description_position = position;
  }
  void resetDescriptionPosition() { description_position = {}; }

  void setListPosition(const common::Point2dI &position) {
    list_position = position;
  }
  void resetListPosition() { list_position = {}; }

  void addItem(const std::shared_ptr<MenuItem>& item);

  std::function<void()> draw;
  std::function<sfxHandle_t(std::int32_t key)> key;
};

} // namespace ui

#endif // RPGXEF_CODE_UI_MENU_MENUFRAMEWORK_H_
