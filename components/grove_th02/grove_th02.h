#pragma once

#include "esphome/components/i2c/i2c.h"
#include "esphome/components/sensor/sensor.h"
#include "esphome/core/component.h"

namespace esphome {
namespace grove_th02 {
static constexpr uint8_t th02I2CDevId = 0x40;
static constexpr uint8_t regStatus = 0x00;
static constexpr uint8_t regDataH = 0x01;
static constexpr uint8_t regDataL = 0x02;
static constexpr uint8_t regConfig = 0x03;
static constexpr uint8_t regId = 0x11;
static constexpr uint8_t statusRdyMask = 0x01;  // poll RDY,0 indicate the conversion is done
static constexpr uint8_t cmdMeasureHumi = 0x01;  // perform a humility measurement
static constexpr uint8_t cmdMeasureTemp = 0x11;  // perform a temperature measurement
static constexpr uint8_t th02WrRegMode = 0xC0;
static constexpr uint8_t th02RdRegMode = 0x80;

class GroveTH02Component : public PollingComponent, public i2c::I2CDevice {
 public:
  void setup() override;
  void update() override;
  void dump_config() override;
  float get_setup_priority() const override;

  void set_temperature_sensor(sensor::Sensor *temperature_sensor) { temperature_sensor_ = temperature_sensor; }
  void set_humidity_sensor(sensor::Sensor *humidity_sensor) { humidity_sensor_ = humidity_sensor; }

 protected:
  sensor::Sensor *temperature_sensor_{nullptr};
  sensor::Sensor *humidity_sensor_{nullptr};

private:
  bool is_ready_();
  float read_temperature_();
  float read_humitidy_();

};

}  // namespace grove_th02
}  // namespace esphome
