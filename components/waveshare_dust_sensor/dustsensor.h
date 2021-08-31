#pragma once

#include "esphome/components/sensor/sensor.h"
#include "esphome/components/voltage_sampler/voltage_sampler.h"
#include "esphome/core/component.h"
#include "esphome/core/defines.h"
#include "esphome/core/esphal.h"

namespace esphome {
namespace waveshare_dust_sensor {
namespace detail {
template<typename T, typename Tl, size_t N> class MovingAvg {
 public:
  MovingAvg &accumulate(T sample) {
    total_ += sample;
    if (num_samples_ < N) {
      samples_[num_samples_++] = sample;
    } else {
      T &oldest = samples_[num_samples_++ % N];
      total_ -= oldest;
      oldest = sample;
    }
    return *this;
  }

  T current() const { return total_ / std::min(num_samples_, N); }

 private:
  T samples_[N];
  size_t num_samples_{0};
  Tl total_{0};
};

}  // namespace detail
class WaveshareDustSensor : public sensor::Sensor, public PollingComponent, public voltage_sampler::VoltageSampler {
 public:
  static constexpr float cov_ratio = 0.2f;          // ug/mmm / mv
  static constexpr float no_dust_voltage = 400.0f;  // mv
  static constexpr float sys_voltage = 5000.0f;

  void update() override;
  void setup() override;
  void dump_config() override;
  float get_setup_priority() const override;
  void set_adc_pin(uint8_t pin) { adc_pin_ = pin; }
  void set_iled_pin(GPIOPin *pin) { iled_pin_ = pin; }
  float sample() override;
#ifdef ARDUINO_ARCH_ESP8266
  std::string unique_id() override;
#endif
 protected:
  uint8_t adc_pin_;
  GPIOPin *iled_pin_ = nullptr;
  float current_value_ = 0;
  detail::MovingAvg<float, float, 1> avg_voltage_{};
};

}  // namespace waveshare_dust_sensor
}  // namespace esphome
