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
  [[maybe_unused]] void toggleWrapAround() { wrap_around = !wrap_around; }
  [[maybe_unused]] void enableWrapAround() { wrap_around = true; }
  [[maybe_unused]] void disableWrapAround() { wrap_around = false; }
  [[maybe_unused]] [[nodiscard]] bool isWrapAround() const {
    return wrap_around;
  }

  [[maybe_unused]] void toggleFullscreen() { fullscreen = !fullscreen; }
  [[maybe_unused]] void enableFullscreen() { fullscreen = true; }
  [[maybe_unused]] void disableFullscreen() { fullscreen = false; }
  [[maybe_unused]] [[nodiscard]] bool isFullscreen() const {
    return fullscreen;
  }

  [[maybe_unused]] void toggleNoBackground() { no_background = !no_background; }
  [[maybe_unused]] void enableNoBackground() { no_background = true; }
  [[maybe_unused]] void disableNoBackground() { no_background = true; }
  [[maybe_unused]] [[nodiscard]] bool isNoBackground() const {
    return no_background;
  }

  [[maybe_unused]] [[nodiscard]] bool isInitialized() const {
    return initialized;
  }

  [[maybe_unused]] void toggleNoSelecting() { no_selecting = !no_selecting; }
  [[maybe_unused]] void enableNoSelecting() { no_selecting = true; }
  [[maybe_unused]] void disableNoSelecting() { no_selecting = false; }
  [[maybe_unused]] [[nodiscard]] bool isNoSelecting() const {
    return no_selecting;
  }

  [[maybe_unused]] [[nodiscard]] std::int32_t getCursor() const {
    return cursor.current;
  }
  [[maybe_unused]] void setCursor(std::int32_t new_cursor);

  [[maybe_unused]] [[nodiscard]] const std::vector<std::shared_ptr<MenuItem>> &
  getMenuItems() const {
    return items;
  }

  [[maybe_unused]] void setTitle(std::int32_t _title) { title = _title; }
  [[maybe_unused]] void resetTitle() { title = 0; }

  [[maybe_unused]] void setFootNote(std::int32_t _foot_note) {
    foot_note = _foot_note;
  }
  [[maybe_unused]] void resetFootNote() { foot_note = 0; }

  [[maybe_unused]] void setTitlePosition(const common::Point2dI &position) {
    title_position = position;
  }
  [[maybe_unused]] void resetTitlePosition() { title_position = {}; }

  [[maybe_unused]] void
  setDescriptionPosition(const common::Point2dI &position) {
    description_position = position;
  }
  [[maybe_unused]] void resetDescriptionPosition() {
    description_position = {};
  }
  [[maybe_unused]] [[nodiscard]] common::Point2dI
  getDescriptionPosition() const {
    return description_position;
  }

  [[maybe_unused]] void setListPosition(const common::Point2dI &position) {
    list_position = position;
  }
  [[maybe_unused]] void resetListPosition() { list_position = {}; }

  [[maybe_unused]] void addItem(const std::shared_ptr<MenuItem> &item);

  std::function<void()> draw;
  std::function<sfxHandle_t(std::int32_t key)> key;
};

} // namespace ui

#endif // RPGXEF_CODE_UI_MENU_MENUFRAMEWORK_H_
