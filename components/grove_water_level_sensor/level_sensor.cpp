#include "level_sensor.h"
#include "esphome/core/log.h"
#include <type_traits>

#define NO_TOUCH 0xF0
#define THRESHOLD 100

namespace esphome {
namespace grove_water_level_sensor {
namespace detail {

template<typename T, size_t N = sizeof(T) * 4, typename std::enable_if<std::is_arithmetic<T>::value, bool>::type = true>
std::string format_bits(T number) {
  char out_string[N + 1] = {0};
  for (size_t idx = 0; idx < N; idx++) {
    if ((number >> (N - idx - 1)) & (T) 0x01) {
      out_string[idx] = '1';
    } else {
      out_string[idx] = '0';
    }
  }
  return std::string{"0b"} + out_string;
}

}  // namespace detail
   // namespace detail
static const char *TAG = "grove_water_level_sensor";
void GroveWaterLevelSensor::setup() {
  ESP_LOGCONFIG(TAG, "Setting up '%s'...", this->name_.c_str());
  if (!this->high_address->read_bytes_raw<12>().has_value()) {
    this->mark_failed();
    return;
  }
}
void GroveWaterLevelSensor::dump_config() {
  LOG_SENSOR("", "Grove Water Level Sensor", this);
  LOG_UPDATE_INTERVAL(this);
}
void GroveWaterLevelSensor::update() {
  auto low = this->low_address->read_bytes_raw<8>();
  auto high = this->high_address->read_bytes_raw<12>();

  if (high.has_value() && low.has_value()) {
    auto high_data = high.value();
    auto low_data = low.value();
    int low_count = 0;
    int high_count = 0;
    uint32_t touch_val = 0;
    uint8_t trig_section = 0;
    int sensorvalue_min = 250;
    int sensorvalue_max = 255;

    ESP_LOGD(TAG, "Bytes: %s%s", esphome::format_hex_pretty(low_data.data(), low_data.size()).c_str(),
             esphome::format_hex_pretty(high_data.data(), high_data.size()).c_str());

    for (int i = 0; i < 8; i++) {
      ESP_LOGV(TAG, "Byte %d %x", i, low_data[i]);
      if (low_data[i] > THRESHOLD) {
        touch_val |= 1 << i;
        low_data[i] = 0x01;
      } else {
        low_data[i] = 0;
      }
    }
    for (int i = 0; i < 12; i++) {
      ESP_LOGV(TAG, "Byte %d %x", (8 + i), high_data[i]);

      if (high_data[i] > THRESHOLD) {
        touch_val |= (uint32_t) 1 << (8 + i);
        high_data[i] = 0x01;
      } else {
        high_data[i] = 0;
      }
    }

    ESP_LOGV(TAG, "Bytes cleared: %s%s", esphome::format_hex_pretty(low_data.data(), low_data.size()).c_str(),
             esphome::format_hex_pretty(high_data.data(), high_data.size()).c_str());
    ESP_LOGD(TAG, "Touch value: %s", detail::format_bits(touch_val).c_str());

    while (touch_val & 0x01) {
      trig_section++;
      touch_val >>= 1;
    }
    float value = (trig_section * 5) / 10.0f;
    this->publish_state(value);

  } else {
    this->mark_failed();
    return;
  }
}

float GroveWaterLevelSensor::get_setup_priority() const { return setup_priority::DATA; }

void GroveWaterLevelSensor::set_high_address(uint8_t addr) { this->high_address->set_i2c_address(addr); }
void GroveWaterLevelSensor::set_low_address(uint8_t addr) { this->low_address->set_i2c_address(addr); }

void GroveWaterLevelSensor::set_i2c_bus(i2c::I2CBus *bus) {
  this->high_address->set_i2c_bus(bus);
  this->low_address->set_i2c_bus(bus);
}

}  // namespace grove_water_level_sensor
}  // namespace esphome