#include "dustsensor.h"
#include "esphome/core/log.h"
#ifdef USE_ESP32
#include "driver/adc.h"
#endif
namespace esphome {
namespace waveshare_dust_sensor {

static const char *TAG = "waveshare_dust_sensor";

void WaveshareDustSensor::setup() {
  ESP_LOGCONFIG(TAG, "Setting up Waveshare Dust Sensor '%s'...", this->get_name().c_str());
  this->iled_pin_->setup();
  this->iled_pin_->digital_write(false);
#ifdef ARDUINO_ARCH_ESP32
  this->adc_->set_attenuation(ADC_ATTEN_DB_11);
#endif
}
void WaveshareDustSensor::dump_config() {
  LOG_SENSOR("", "Waveshare Dust Sensor", this);
  LOG_SENSOR(TAG, "   ADC", this->adc_);
  LOG_PIN("  ILED Pin:", this->iled_pin_);
  LOG_UPDATE_INTERVAL(this);
}
float WaveshareDustSensor::get_setup_priority() const { return setup_priority::DATA; }
void WaveshareDustSensor::update() {
  this->iled_pin_->digital_write(true);
  delayMicroseconds(100);
  // Note: The output voltage has been divided (see schematic), so that the voltage measurement should x 11 to get the
  // actual voltage.
  float volts = this->adc_->sample() * 11.0f * 1000.0f;  // to mV
  this->iled_pin_->digital_write(false);
  ESP_LOGD(TAG, "'%s': Got voltage=%.2fmV", this->get_name().c_str(), volts);

  float ppm_value = 0;
  if (volts >= no_dust_voltage) {
    volts -= no_dust_voltage;
    ppm_value = volts * cov_ratio;
  }
  this->publish_state(ppm_value);
}

}  // namespace waveshare_dust_sensor
}  // namespace esphome
