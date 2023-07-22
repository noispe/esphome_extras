#pragma once

#include "elements.h"

namespace esphome {
namespace ui_components {
class ShapeElement : public BaseElement {
 public:
  enum class ShapeType {
    CIRCLE,
    FILLED_CIRCLE,
    RECTANGLE,
    FILLED_RECTANGLE,
    LINE,
    ROUND_RECTANGLE,
    ROUND_FILLED_RECTANGLE
  };

  void draw(display::Display &disp) override;
  void set_shape_type(ShapeType shapetype) { shapetype_ = shapetype; }
  void set_radius(int r) { radius_ = r; }

 private:
  int radius_ = 0;
  ShapeType shapetype_;
};
}  // namespace ui_components
}  // namespace esphome
