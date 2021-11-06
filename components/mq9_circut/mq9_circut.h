#pragma once

#include "esphome/components/sensor/sensor.h"
#include "esphome/components/adc/adc_sensor.h"
#include "esphome/core/component.h"
#include "esphome/core/defines.h"
#include "calibration.h"

namespace esphome {
namespace mq9_circut {

namespace detail {
template<typename T, typename Tl, size_t N> class MovingAvg {
 public:
  static constexpr size_t size = N;
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

  MovingAvg &reset() {
    num_samples_ = 0;
    total_ = 0;
    return *this;
  }

 private:
  T samples_[N];
  size_t num_samples_{0};
  Tl total_{0};
};

}  // namespace detail
class Mq9Circut : public sensor::Sensor,
                  public Component {
public:
  static constexpr float Vc = 5.0f;
  static constexpr float Rl = 10000.0f;
  void update_co();
  void update_tvoc();
  void setup() override;
  void dump_config() override;
  float get_setup_priority() const override;
  void set_adc(adc::ADCSensor *adc) { adc_ = adc; }
  void set_control_pin(GPIOPin *pin) { control_pin_ = pin; }
  void set_tvoc_sensor(sensor::Sensor *sensor) { tvoc_sensor_ = sensor; }
  void set_co_sensor(sensor::Sensor *sensor) { co_sensor_ = sensor; }
  void set_calibration_sensor(sensor::Sensor *sensor) { calibration_sensor_ = sensor; }
  void set_r0(float r0) { r0_ = r0; }

protected:
  void start_calibration();
  float sample_calibration();
  float calculate_ppm(calibration::gas_type type, float VRl);
  void toggle_burnoff();
  uint count_ = 0;
  adc::ADCSensor *adc_ = nullptr;
  bool burnoff_ = false;
  float r0_ = 0.0f;
  detail::MovingAvg<float, float, 120> calibration_accumulator_{};
  GPIOPin *control_pin_ = nullptr;
  sensor::Sensor *tvoc_sensor_{nullptr};
  sensor::Sensor *co_sensor_{nullptr};
  sensor::Sensor *calibration_sensor_{nullptr};
};

} // namespace mq9_circut
} // namespace esphome
