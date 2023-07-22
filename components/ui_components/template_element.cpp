#include "template_element.h"

namespace esphome {

void ui_components::TemplateElement::draw(display::Display &disp) {
  if (border_) {
    disp.rectangle(x_, y_, width_, height_, get_fg_color());
  }
  disp.filled_rectangle(x(), y(), width(), height(), get_bg_color());
  drawer_(disp, x(), y(), width(), height(), alignment_, get_fg_color(), get_bg_color());
}
}  // namespace esphome
