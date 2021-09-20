#pragma once

#include "esphome/components/sensor/sensor.h"
#include "esphome/components/voltage_sampler/voltage_sampler.h"
#include "esphome/core/component.h"
#include "esphome/core/defines.h"
#include "esphome/core/esphal.h"
#include "calibration.h"

namespace esphome {
namespace mq9_circut {

class Mq9Circut : public sensor::Sensor,
                  public Component,
                  public voltage_sampler::VoltageSampler {
public:
  void update_co();
  void update_tvoc();
  void setup() override;
  void dump_config() override;
  float get_setup_priority() const override;
  float sample() override;
  void set_adc_pin(uint8_t pin) { adc_pin_ = pin; }
  void set_control_pin(GPIOPin *pin) { control_pin_ = pin; }
  void set_tvoc_sensor(sensor::Sensor *sensor) { tvoc_sensor_ = sensor; }
  void set_co_sensor(sensor::Sensor *sensor) { co_sensor_ = sensor; }
  void set_r0(float r0) { r0_ = r0; }
#ifdef ARDUINO_ARCH_ESP8266
  std::string unique_id() override;
#endif
protected:
  void start_calibration();
  void finish_calibration();
  float calculate_ppm(calibration::gas_type type, float voltage);
  void toggle_burnoff();
  uint count_ = 0;
  uint8_t adc_pin_;
  bool burnoff_ = false;
  bool calibrating_ = false;
  int calibration_counter_ = 1000;
  float calibration_accumulator_ = 0.0f;
  float r0_ = 0.0f;
  GPIOPin *control_pin_ = nullptr;
  sensor::Sensor *tvoc_sensor_{nullptr};
  sensor::Sensor *co_sensor_{nullptr};
};

} // namespace mq9_circut
} // namespace esphome
