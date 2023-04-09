// Implementation based on aht10

#include "grove_th02.h"
#include "esphome/core/hal.h"
#include "esphome/core/log.h"

namespace esphome {
namespace grove_th02 {

static const char *const TAG = "grove_th02";

void GroveTH02Component::setup() {
  ESP_LOGCONFIG(TAG, "Setting up TH02...");
  auto identity = this->read_byte(regId).value_or(0x00);

  if (identity == 0b0101000) {
    ESP_LOGD(TAG, "Configured.");
  } else {
    ESP_LOGE(TAG, "Unknown id %x", identity);
    this->mark_failed();
  }
}

void GroveTH02Component::update() {
  if (this->temperature_sensor_ != nullptr) {
    this->temperature_sensor_->publish_state(this->read_temperature_());
  }
  if (this->humidity_sensor_ != nullptr) {
    this->humidity_sensor_->publish_state(this->read_humitidy_());
  }
  this->status_clear_warning();
}

float GroveTH02Component::get_setup_priority() const { return setup_priority::DATA; }

void GroveTH02Component::dump_config() {
  ESP_LOGCONFIG(TAG, "TH02:");
  LOG_I2C_DEVICE(this);
  if (this->is_failed()) {
    ESP_LOGE(TAG, "Communication with TH02 failed!");
  }
  LOG_SENSOR("  ", "Temperature", this->temperature_sensor_);
  LOG_SENSOR("  ", "Humidity", this->humidity_sensor_);
}

bool GroveTH02Component::is_ready_() {
  auto status = this->read_byte(regStatus).value_or(0xFF);
  return !(status & statusRdyMask);
}

float GroveTH02Component::read_temperature_() {
  this->write_byte(regConfig, cmdMeasureTemp);
  while (!this->is_ready_())
    ;

  uint16_t data = this->read_byte(regDataH).value_or(0x00) << 8;
  data |= this->read_byte(regDataL).value_or(0x00);
  data = data >> 2;
  return (data / 32.0f) - 50.0f;
}

float GroveTH02Component::read_humitidy_() {
  this->write_byte(regConfig, cmdMeasureHumi);
  while (!this->is_ready_())
    ;

  uint16_t data = this->read_byte(regDataH).value_or(0x00) << 8;
  data |= this->read_byte(regDataL).value_or(0x00);
  data = data >> 4;
  return (data / 16.0f) - 24.0f;
}

}  // namespace grove_th02
}  // namespace esphome
