#include "shape_element.h"

namespace esphome {

void ui_components::ShapeElement::draw(display::DisplayBuffer &disp) {
  if (border_) {
    disp.rectangle(x_, y_, width_, height_, get_fg_color());
  }
  disp.filled_rectangle(x(), y(), width(), height(), get_bg_color());

  switch (shapetype_) {
    case ShapeType::CIRCLE:
      disp.circle(x() + (width() / 2), y() + (height() / 2), std::min(height(), width()), get_fg_color());
      break;
    case ShapeType::FILLED_CIRCLE:
      disp.filled_circle(x() + (width() / 2), y() + (height() / 2), std::min(height(), width()), get_fg_color());
      break;
    case ShapeType::RECTANGLE:
      disp.rectangle(x(), y(), width(), height(), get_fg_color());
      break;
    case ShapeType::FILLED_RECTANGLE:
      disp.filled_rectangle(x(), y(), width(), height(), get_fg_color());
      break;
    case ShapeType::ROUND_RECTANGLE:
      Primitives::round_rect(disp, x(), y(), width(), height(), radius_, get_fg_color());
      break;
    case ShapeType::ROUND_FILLED_RECTANGLE:
      Primitives::filled_round_rect(disp, x(), y(), width(), height(), radius_, get_fg_color());
      break;
    case ShapeType::LINE:
      if (height_ == 0) {
        disp.horizontal_line(x(), y(), width(), get_fg_color());
      } else if (width_ == 0) {
        disp.vertical_line(x(), y(), height(), get_fg_color());
      } else {
        disp.line(x(), y(), x() + width(), y() + height(), get_fg_color());
      }
      break;
    default:
      break;
  }
}

}  // namespace esphome
