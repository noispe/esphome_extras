#include "sht_20.h"
#include "esphome/core/log.h"

#define SHT20_ADDR 0x40
static const char *TAG = "sht20";

namespace esphome {
namespace sht20 {

void sht20_component::dump_config() {
  ESP_LOGCONFIG(TAG, "ADE7953:");
  LOG_I2C_DEVICE(this);
  LOG_UPDATE_INTERVAL(this);
}

float sht20_component::read_humidity() {
  uint16_t rawHumidity = 0;

  if (!read_byte_16(TRIGGER_HUMD_MEASURE_NOHOLD, &rawHumidity, 10)) {
    ESP_LOGE(TAG, "error reading TRIGGER_HUMD_MEASURE_NOHOLD");
    return 0;
  }
  return -6.0 + rawHumidity * (125.0 / 65536.0);
}

void sht20_component::update() {
  if (this->temperature_sensor_ != nullptr) {
    this->temperature_sensor_->publish_state(this->read_temperature());
  }
  if (this->humidity_sensor_ != nullptr) {
    this->humidity_sensor_->publish_state(this->read_humidity());
  }
}

float sht20_component::read_temperature() {
  uint16_t rawTemperature = 0;

  if (!read_byte_16(TRIGGER_TEMP_MEASURE_NOHOLD, &rawTemperature, 10)) {
    ESP_LOGE(TAG, "error reading TRIGGER_TEMP_MEASURE_NOHOLD");
    return 0;
  }

  return -46.85 + (float) rawTemperature * (175.72 / 65536.0);
}

void sht20_component::set_resolution(sht20_component::Resolutions resolution) {
  uint8_t userRegister = 0;
  if (read_byte(READ_USER_REG, &userRegister)) {
    userRegister &= B01111110;
    userRegister |= (resolution & B10000001);
  }
  write_byte(WRITE_USER_REG, userRegister);
}
}  // namespace sht20
}  // namespace esphome
