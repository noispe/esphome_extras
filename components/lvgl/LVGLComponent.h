#pragma once

#include "esphome/core/component.h"
#include "lvgl.h"

namespace esphome {
namespace lvgl {
class LVGLComponent : public public Component {
 public:
  float get_setup_priority() const override { return setup_priority::DATA; };
  void setup() override;
  void IRAM_ATTR loop() override;

};
}
}

