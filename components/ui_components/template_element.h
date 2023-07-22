#pragma once

#include "elements.h"

namespace esphome {
namespace ui_components {

class TemplateElement : public BaseElement {
 public:
  using drawing_template_t =
      std::function<void(display::Display &, int, int, int, int, display::TextAlign, Color, Color)>;
  void draw(display::Display &disp) override;
  void set_drawer(drawing_template_t drawer) { drawer_ = drawer; }

 private:
  drawing_template_t drawer_;
};
}  // namespace ui_components
}  // namespace esphome
