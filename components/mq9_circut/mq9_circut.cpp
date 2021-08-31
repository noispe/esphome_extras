#include "mq9_circut.h"
#include "esphome/core/log.h"

namespace esphome {
namespace mq9_circut {

static const char *TAG = "mq9_circut";

void Mq9Circut::update_co() {
  if (this->co_sensor_ == nullptr) {
    return;
  }
  float volts = this->sample();
  ESP_LOGD(TAG, "'%s': Got voltage=%.2fV", this->co_sensor_->get_name().c_str(), volts);
  this->co_sensor_->publish_state(this->calculate_ppm(gas_type::CO, volts));
}

void Mq9Circut::update_tvoc() {
  if (this->tvoc_sensor_ == nullptr) {
    return;
  }
  float volts = this->sample();
  ESP_LOGD(TAG, "'%s': Got voltage=%.2fV", this->tvoc_sensor_->get_name().c_str(), volts);
  this->tvoc_sensor_->publish_state(this->calculate_ppm(gas_type::LPG, volts));
}

void Mq9Circut::calibrate() {
  ESP_LOGD(TAG, "Start R0 calibration");
  this->burnoff_ = false;
  this->toggle_burnoff();
  delayMicroseconds(500);

  float sensor_values = 0.0f;     // Define variable for analog readings
  for (int x = 0; x < 1000; x++)  // Start for loop
  {
    sensor_values = sensor_values + this->sample();  // Add analog values of sensor 500 times
    delayMicroseconds(10);
  }
  float sensor_volt = sensor_values / 1000.0;          // Take average of readings
  float rs_air = ((5.0 * 10.0) / sensor_volt) - 10.0;  // Calculate RS in fresh air
  this->r0_ = rs_air / 4.4;                            // Calculate R0
  ESP_LOGD(TAG, "Calibrated R0=%.2f", this->r0_);
  this->toggle_burnoff();
}

float Mq9Circut::calculate_ppm(gas_type type, float voltage) {
  float range = this->burnoff_ ? 5.0 : 1.5;
  float rs_gas = ((range * 10.0) / voltage) - 10.0;  // Get value of RS in a gas
  double ratio = rs_gas / this->r0_;                 // Get ratio rs_gas/rs_air

  switch (type) {
    case gas_type::CO:
      return pow(10, ((log(ratio) - 1.24) / -0.45));
      break;

    case gas_type::LPG:
      return pow(10, ((log(ratio) - 1.37) / -0.46));
      break;

    case gas_type::NH4:
      return pow(10, ((log(ratio) - 3.35) / -0.37));
      break;

    default:
      return 0;
      break;
  }
}

void Mq9Circut::setup() {
  ESP_LOGCONFIG(TAG, "Setting up MQ9 circut '%s'...", this->get_name().c_str());
  this->control_pin_->setup();
  this->control_pin_->digital_write(false);
  if (this->r0_ == 0) {
    this->calibrate();
  } else {
    ESP_LOGD(TAG, "No R0 calibration needed, using %.2f", this->r0_);
  }
  set_interval(30000, [this]() {
    this->count_++;
    if (this->burnoff_ && this->count_ % 2 == 0) {
      ESP_LOGD(TAG, "Sample tvoc %d, %d", this->burnoff_, this->count_);
      this->toggle_burnoff();
      this->update_tvoc();
      this->count_ = 0;
    } else if (!this->burnoff_ && this->count_ % 3 == 0) {
      ESP_LOGD(TAG, "Sample co %d, %d", this->burnoff_, this->count_);
      this->update_co();
      this->count_ = 0;
      this->toggle_burnoff();
    }
  });
}

void Mq9Circut::dump_config() {
  ESP_LOGCONFIG(TAG, "MQ9 circut:");
  ESP_LOGCONFIG(TAG, "ADC Pin: %u", this->adc_pin_);
  ESP_LOGCONFIG(TAG, "R0: %0.2f", this->r0_);
  LOG_PIN("Control Pin:", this->control_pin_);
  LOG_SENSOR(TAG, "TVOC", this->tvoc_sensor_);
  LOG_SENSOR(TAG, "CO", this->co_sensor_);
}

void Mq9Circut::toggle_burnoff() {
  this->burnoff_ = !this->burnoff_;
#ifdef ARDUINO_ARCH_ESP32
  if (this->burnoff_) {
    analogSetPinAttenuation(this->adc_pin_, ADC_11db);
  } else {
    analogSetPinAttenuation(this->adc_pin_, ADC_2_5db);
  }
#endif
  this->control_pin_->digital_write(!this->burnoff_);
}

float Mq9Circut::sample() {
#ifdef ARDUINO_ARCH_ESP32
  float value_v = analogRead(this->adc_pin_) / 4095.0f;  // NOLINT
  if (this->burnoff_) {
    value_v *= 3.9;
  } else {
    value_v *= 1.1;
  }
  return value_v;
#endif

#ifdef ARDUINO_ARCH_ESP8266
  return analogRead(this->adc_pin_) / 1024.0f;  // NOLINT
#endif
}

float Mq9Circut::get_setup_priority() const { return setup_priority::DATA; }

#ifdef ARDUINO_ARCH_ESP8266
std::string Mq9Circut::unique_id() { return get_mac_address() + "-mq9"; }
#endif

}  // namespace mq9_circut

}  // namespace esphome
