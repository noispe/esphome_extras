#include "mq9_circut.h"
#include "esphome/core/log.h"
#ifdef USE_ESP32
#include "driver/adc.h"
#endif

namespace esphome {
namespace mq9_circut {

static const char *TAG = "mq9_circut";

void Mq9Circut::update_co() {
  if (this->co_sensor_ == nullptr) {
    return;
  }
  float volts = this->adc_->sample();
  ESP_LOGD(TAG, "'%s': Got voltage=%.2fV", this->co_sensor_->get_name().c_str(), volts);
  this->co_sensor_->publish_state(this->calculate_ppm(calibration::gas_type::CO, volts));
}

void Mq9Circut::update_tvoc() {
  if (this->tvoc_sensor_ == nullptr) {
    return;
  }
  float volts = this->adc_->sample();
  ESP_LOGD(TAG, "'%s': Got voltage=%.2fV", this->tvoc_sensor_->get_name().c_str(), volts);
  this->tvoc_sensor_->publish_state(this->calculate_ppm(calibration::gas_type::LPG, volts) * 1000);
}

float Mq9Circut::calculate_ppm(calibration::gas_type type, float VRl) {
  float Vc = type == calibration::gas_type::CO ? Vcl : Vch;
  float rs_gas = ((Vc - VRl) / VRl) * Rl;  // Get value of RS in a gas
  float ratio = rs_gas / this->r0_;        // Get ratio rs_gas/rs_air
  float ppm = powf(10, (log10f(ratio) - calibration::setpoints[type].b) / calibration::setpoints[type].m);

  ESP_LOGCONFIG(TAG, "calculate_ppm %d '%.2fV' scale: '%.2f'.", (int) type, VRl, ppm);
  return ppm;
}

void Mq9Circut::setup() {
  ESP_LOGCONFIG(TAG, "Setting up MQ9 circut '%s'...", this->get_name().c_str());
#ifdef ARDUINO_ARCH_ESP32
  this->adc_->set_attenuation(ADC_ATTEN_DB_11);
#endif
  this->control_pin_->setup();
  this->control_pin_->digital_write(false);
  if (this->calibration_sensor_ != nullptr) {
    this->start_calibration();
    set_interval(30000, [this]() { this->calibration_sensor_->publish_state(this->sample_calibration()); });
  } else {
    set_interval(30000, [this]() {
      this->count_++;
      if (this->burnoff_ && this->count_ > 2) {
        ESP_LOGD(TAG, "Sample tvoc %d, %d", this->burnoff_, this->count_);
        this->toggle_burnoff();
        this->update_tvoc();
        this->count_ = 0;
      } else if (!this->burnoff_ && this->count_ > 3) {
        ESP_LOGD(TAG, "Sample co %d, %d", this->burnoff_, this->count_);
        this->update_co();
        this->count_ = 0;
        this->toggle_burnoff();
      }
    });
  }
}

void Mq9Circut::dump_config() {
  ESP_LOGCONFIG(TAG, "MQ9 circut:");
  LOG_SENSOR(TAG, "ADC", this->adc_);
  ESP_LOGCONFIG(TAG, "R0: %0.2f", this->r0_);
  LOG_PIN("Control Pin:", this->control_pin_);
  LOG_SENSOR(TAG, "TVOC", this->tvoc_sensor_);
  LOG_SENSOR(TAG, "CO", this->co_sensor_);
}

void Mq9Circut::toggle_burnoff() {
  this->burnoff_ = !this->burnoff_;
  this->control_pin_->digital_write(!this->burnoff_);
}

float Mq9Circut::get_setup_priority() const { return setup_priority::DATA; }

void Mq9Circut::start_calibration() {
  ESP_LOGD(TAG, "Start R0 calibration sample");
  this->burnoff_ = true;
  this->control_pin_->digital_write(!this->burnoff_);
  this->calibration_accumulator_.reset();
  delayMicroseconds(500);
}

float Mq9Circut::sample_calibration() {
  ESP_LOGD(TAG, "Finish R0 calibration sample");
  this->calibration_accumulator_.accumulate(this->adc_->sample());
  float r0 = 0.0f;
  float sensor_volt = this->calibration_accumulator_.current();
  if (sensor_volt != 0.0f) {
    float rs_air = ((Vch * Rl) / sensor_volt) - Rl;  // Calculate RS in fresh air
    r0 = rs_air / 10.0f;                            // Calculate R0 for air
    ESP_LOGD(TAG, "Calibrated R0=%.2f voltage=%.2f", r0, sensor_volt);
  }
  return r0;
}

}  // namespace mq9_circut

}  // namespace esphome
