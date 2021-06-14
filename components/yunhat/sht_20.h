#pragma once

#include "esphome/core/component.h"
#include "esphome/components/sensor/sensor.h"
#include "esphome/components/i2c/i2c.h"

namespace esphome {
namespace sht20 {
class sht20_component : public PollingComponent, public i2c::I2CDevice {
 public:
  enum Resolutions {
    RH08_TEMP12 = 0x01,
    RH10_TEMP13 = 0x80,
    RH11_TEMP11 = 0x81,
    RH12_TEMP14 = 0x00,
  };

  static constexpr uint8_t TRIGGER_TEMP_MEASURE_NOHOLD = 0xF3;
  static constexpr uint8_t TRIGGER_HUMD_MEASURE_NOHOLD = 0xF5;
  static constexpr uint8_t WRITE_USER_REG = 0xE6;
  static constexpr uint8_t READ_USER_REG = 0xE7;

  void set_temperature_sensor(sensor::Sensor *temperature_sensor) { temperature_sensor_ = temperature_sensor; }
  void set_humidity_sensor(sensor::Sensor *humidity_sensor) { humidity_sensor_ = humidity_sensor; }

  void dump_config() override;
  void update() override;
  float get_setup_priority() const override { return setup_priority::DATA; }
  void set_resolution(Resolutions resolutions);
  float read_humidity();
  float read_temperature();

 private:
  sensor::Sensor *temperature_sensor_{nullptr};
  sensor::Sensor *humidity_sensor_{nullptr};
};
}  // namespace sht20
}  // namespace esphome
