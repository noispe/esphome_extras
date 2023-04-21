#pragma once

#include "esphome/core/component.h"
#include "esphome/core/defines.h"

#include "esphome/components/binary_sensor/binary_sensor.h"

#ifdef USE_SENSOR
#include "esphome/components/sensor/sensor.h"
#endif

#ifdef USE_TEXT_SENSOR
#include "esphome/components/text_sensor/text_sensor.h"
#endif

#ifdef USE_SELECT
#include "esphome/components/select/select.h"
#endif

#ifdef USE_NUMBER
#include "esphome/components/number/number.h"
#endif

#include <vector>

namespace esphome {
namespace group_has_state {

class GroupHasState : public binary_sensor::BinarySensorInitiallyOff, public PollingComponent {
 public:
  void dump_config() override;
  float get_setup_priority() const override { return setup_priority::DATA; }
  void update() override;

  void monitor_state(binary_sensor::BinarySensor *s) { this->binary_sensors_.push_back(s); }
  std::vector<binary_sensor::BinarySensor *> binary_sensors_;

#ifdef USE_SENSOR
  void monitor_state(sensor::Sensor *s) { this->sensors_.push_back(s); }
  std::vector<sensor::Sensor *> sensors_;
#endif

#ifdef USE_TEXT_SENSOR
  void monitor_state(text_sensor::TextSensor *s) { this->text_sensors_.push_back(s); }
  std::vector<text_sensor::TextSensor *> text_sensors_;
#endif

#ifdef USE_SELECT
  void monitor_state(select::Select *s) { this->select_sensors_.push_back(s); }
  std::vector<select::Select *> select_sensors_;
#endif

#ifdef USE_NUMBER
  void monitor_state(number::Number *s) { this->number_sensors_.push_back(s); }
  std::vector<number::Number *> number_sensors_;
#endif
};

}  // namespace group_has_state
}  // namespace esphome
