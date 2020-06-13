//
// Created by Walter on 24.04.2020.
//

#ifndef RPGXEF_CODE_UI_MENU_MENUCOLORS_H_
#define RPGXEF_CODE_UI_MENU_MENUCOLORS_H_

#include <common/Color.h>

namespace ui {

struct colors {
  static inline constexpr const common::Color menu_text{1.0, 1.0, 1.0, 1.0};
  static inline constexpr const common::Color menu_dim{0.0, 0.0, 0.0, 0.75};
  static inline constexpr const common::Color white{1.0, 1.0, 1.0, 1.0};
  static inline constexpr const common::Color yellow{1.0, 1.0, 0.0, 1.0};
  static inline constexpr const common::Color blue{0.0, 0.0, 1.0, 1.0};
  static inline constexpr const common::Color light_orange{1.0, 0.68, 0.0, 1.0};
  static inline constexpr const common::Color orange{1.0, 0.43, 0.0, 1.0};
  static inline constexpr const common::Color red{1.0, 0.0, 0.0, 1.0};
  static inline constexpr const common::Color dim{0.0, 0.0, 0.0, 0.25};

  static inline constexpr const common::Color pulse_color{1.0, 1.0, 1.0, 1.0};
  static inline constexpr const common::Color text_disabled{0.5, 0.5, 0.5, 1.0};
  static inline constexpr const common::Color text_normal{1.0, 0.43, 0.0, 1.0};
  static inline constexpr const common::Color text_highlight{1.0, 1.0, 0.0,
                                                             1.0};
  static inline constexpr const common::Color listbar_color{1.0, 0.43, 0.0,
                                                            0.3};
  static inline constexpr const common::Color text_status{1.0, 1.0, 1.0, 1.0};
};

} // namespace ui

#endif // RPGXEF_CODE_UI_MENU_MENUCOLORS_H_
