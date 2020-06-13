#include "MenuTextP.h"
#include <ui/Atoms.h>

namespace ui {

void MenuTextP::init() {
  const auto size_scale = Atoms::ProportionalSizeScale(style);
  auto x = common.position.x;
  auto y = common.position.y;
  const auto w =
      Atoms::ProportionalStringWidth(text, UI_SMALLFONT) * size_scale;
  const auto h = PROP_HEIGHT * size_scale;
  if (common.right_justify()) {
    x -= w;
  } else if (common.center_justify()) {
    x -= w / 2;
  }

  common.rectangle.left = x - PROP_GAP_WIDTH * size_scale;
  common.rectangle.right = x + w + PROP_GAP_WIDTH * size_scale;
  common.rectangle.top = y;
  common.rectangle.bottom = y + h;
}

} // namespace ui
