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
  ESP_LOGD(TAG, "'%s': Got voltage=%.2fV", this->co_sensor_->get_name().c_str(),
           volts);
  this->co_sensor_->publish_state(this->calculate_ppm(calibration::gas_type::CO, volts));
}

void Mq9Circut::update_tvoc() {
  if (this->tvoc_sensor_ == nullptr) {
    return;
  }
  float volts = this->sample();
  ESP_LOGD(TAG, "'%s': Got voltage=%.2fV",
           this->tvoc_sensor_->get_name().c_str(), volts);
  this->tvoc_sensor_->publish_state(this->calculate_ppm(calibration::gas_type::LPG, volts) * 1000);
}

float Mq9Circut::calculate_ppm(calibration::gas_type type, float voltage) {
  float range = this->burnoff_ ? 5.0f : 1.5f;
  float rs_gas =
      ((range * 10.0f) / voltage) - 10.0f; // Get value of RS in a gas
  float ratio = rs_gas / this->r0_;        // Get ratio rs_gas/rs_air

  float ppm = powf(10, (log10f(ratio) - calibration::setpoints[type].b) / calibration::setpoints[type].m);

  ESP_LOGCONFIG(TAG, "calculate_ppm %d '%.2fV' scale: '%.2f'.", (int)type,
                voltage, ppm);
  return ppm;
}

void Mq9Circut::setup() {
  ESP_LOGCONFIG(TAG, "Setting up MQ9 circut '%s'...", this->get_name().c_str());
  this->control_pin_->setup();
  this->control_pin_->digital_write(false);
  if (this->r0_ == 0) {
    this->start_calibration();
  }

  set_interval(300, [this]() {
    if (this->calibrating_) {
      this->count_++;
      if (this->count_ < 10) { // warm up 30sec
        return;
      }
      this->calibration_accumulator_ += this->sample();
      if (this->count_ > 810) { // 2min
        this->finish_calibration();
        this->count_ = 0;
      }
      return;
    }
    if (this->r0_ == 0) {
      this->start_calibration();
      return;
    }
    this->count_++;
    if (this->burnoff_ && this->count_ > 200) {
      ESP_LOGD(TAG, "Sample tvoc %d, %d", this->burnoff_, this->count_);
      this->toggle_burnoff();
      this->update_tvoc();
      this->count_ = 0;
    } else if (!this->burnoff_ && this->count_ > 300) {
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
  float value_v = analogRead(this->adc_pin_) / 4095.0f; // NOLINT
  if (this->burnoff_) {
    value_v *= 3.9f;
  } else {
    value_v *= 1.1f;
  }
  return value_v;
#endif

#ifdef ARDUINO_ARCH_ESP8266
  return analogRead(this->adc_pin_) / 1024.0f; // NOLINT
#endif
}

float Mq9Circut::get_setup_priority() const { return setup_priority::DATA; }

void Mq9Circut::start_calibration() {
  ESP_LOGD(TAG, "Start R0 calibration");
  this->calibrating_ = true;
  this->calibration_accumulator_ = 0.0f; // Define variable for analog readings
  this->burnoff_ = false;
  this->toggle_burnoff();
  delayMicroseconds(500);
}

void Mq9Circut::finish_calibration() {
  ESP_LOGD(TAG, "Finish R0 calibration");
  float sensor_volt =
      this->calibration_accumulator_ / 800.0f; // Take average of readings
  if (sensor_volt != 0.0f) {
    float rs_air =
        ((5.0f * 10.0f) / sensor_volt) - 10.0f; // Calculate RS in fresh air
    this->r0_ = rs_air / 4.4f;                  // Calculate R0
    ESP_LOGD(TAG, "Calibrated R0=%.2f", this->r0_);
  } else {
    ESP_LOGD(TAG, "Failed sensor voltage=%.2f (%.2f)", sensor_volt,
             this->calibration_accumulator_);
  }
  this->toggle_burnoff();
  this->calibrating_ = false;
}

#ifdef ARDUINO_ARCH_ESP8266
std::string Mq9Circut::unique_id() { return get_mac_address() + "-mq9"; }
#endif

} // namespace mq9_circut

} // namespace esphome
