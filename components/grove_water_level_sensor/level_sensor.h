#pragma once

#include "esphome/components/sensor/sensor.h"
#include "esphome/components/i2c/i2c.h"
#include "esphome/core/component.h"
#include "esphome/core/defines.h"

namespace esphome {
namespace grove_water_level_sensor {

class GroveWaterLevelSensor : public sensor::Sensor, public PollingComponent {
 public:
  void update() override;
  void setup() override;
  void dump_config() override;
  float get_setup_priority() const override;
  void set_high_address(uint8_t addr);
  void set_low_address(uint8_t addr);
  void set_i2c_bus(i2c::I2CBus *bus);

 protected:
  i2c::I2CDevice *high_address = new i2c::I2CDevice();
  i2c::I2CDevice *low_address = new i2c::I2CDevice();
};

}  // namespace grove_water_level_sensor
}  // namespace esphome
