#pragma once

#include "esphome/core/component.h"
#include "esphome/core/defines.h"
#include "esphome/core/macros.h"
#include "esphome/core/helpers.h"
#include "esphome/components/text_sensor/text_sensor.h"

namespace esphome {
namespace restart_reason {

class RestartReasonComponent : public text_sensor::TextSensor, public Component {
 public:
  void setup() override;
  void dump_config() override;
  float get_setup_priority() const override;

 private:
  static std::string get_startup_reason_();
};

}  // namespace restart_reason
}  // namespace esphome
