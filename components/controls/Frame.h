#pragma once
#include "esphome/core/component.h"
#include "esphome/components/display/display_buffer.h"

namespace esphome {
namespace controls {
struct Geometry {
  int x = 0;
  int y = 0;
  int width = 0;
  int height = 0;
  Geometry draw_area(int padding, int margin);
  Geometry border_area(int padding, int margin);
};

class Frame {
 public:
  virtual ~Frame() = default;
  virtual void refresh() = 0;

  void set_display(display::DisplayBuffer *display);
  void set_geometry(const Geometry &g);

  Geometry geometry{};

 protected:
  display::DisplayBuffer *display_ = nullptr;
};

}  // namespace controls

}  // namespace esphome
