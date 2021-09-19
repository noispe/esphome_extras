#include "dustsensor.h"
#include "esphome/core/log.h"

namespace esphome {
namespace waveshare_dust_sensor {

static const char *TAG = "waveshare_dust_sensor";

void WaveshareDustSensor::setup() {
  ESP_LOGCONFIG(TAG, "Setting up Waveshare Dust Sensor '%s'...", this->get_name().c_str());
  this->iled_pin_->setup();
  this->iled_pin_->digital_write(false);
#ifdef ARDUINO_ARCH_ESP32
  analogSetPinAttenuation(this->adc_pin_, ADC_11db);
#endif
}
void WaveshareDustSensor::dump_config() {
  LOG_SENSOR("", "Waveshare Dust Sensor", this);
  ESP_LOGCONFIG(TAG, "  ADC Pin: %u", this->adc_pin_);
  LOG_PIN("  ILED Pin:", this->iled_pin_);
  LOG_UPDATE_INTERVAL(this);
}
float WaveshareDustSensor::get_setup_priority() const { return setup_priority::DATA; }
void WaveshareDustSensor::update() {
  this->iled_pin_->digital_write(true);
  delayMicroseconds(280);
  // Note: The output voltage has been divided (see schematic), so that the voltage measurement should x 11 to get the actual voltage.
  float volts = this->sample() * 11.0f * 1000.0f;  // to mV
  this->iled_pin_->digital_write(false);
  ESP_LOGD(TAG, "'%s': Got voltage=%.2fV", this->get_name().c_str(), volts);

  float ppm_value = 0;
  if (volts >= no_dust_voltage) {
    volts -= no_dust_voltage;
    ppm_value = volts * cov_ratio;
  }
  this->publish_state(ppm_value);
}

float WaveshareDustSensor::sample() {
#ifdef ARDUINO_ARCH_ESP32
  //avg_voltage_.accumulate(analogRead(this->adc_pin_) / 4095.0f * 1.1);  // NOLINT
  return analogRead(this->adc_pin_) / 4095.0f * 3.9;  // NOLINT
#endif

#ifdef ARDUINO_ARCH_ESP8266
  //avg_voltage_.accumulate(analogRead(this->adc_pin_) / 1024.0);  // NOLINT
  return analogRead(this->adc_pin_) / 1024.0;  // NOLINT
#endif
  // return avg_voltage_.current();
}

#ifdef ARDUINO_ARCH_ESP8266
std::string WaveshareDustSensor::unique_id() { return get_mac_address() + "-wsds"; }
#endif

}  // namespace waveshare_dust_sensor
}  // namespace esphome
