#pragma once

#include "esphome/core/component.h"
#include "esphome/components/i2c/i2c.h"
#include "esphome/components/light/addressable_light.h"
#include "esphome/core/log.h"

namespace esphome {
namespace yunhat {
class YunHatLights : public light::AddressableLight, public i2c::I2CDevice {
  constexpr static const char *TAG = "yunhat";
  constexpr static int32_t led_count = 14;
  constexpr static uint8_t led_register = 0x01;
  std::array<light::ESPColor, led_count> leds_;
  std::array<uint8_t, led_count> effect_data_;
  std::vector<light::ESPColorView> state_;

 public:
  void clear_effect_data() override;

  void dump_config() override {
    ESP_LOGCONFIG(TAG, "YUN Hat:");
    LOG_I2C_DEVICE(this);
  }

  void setup() override {
    ESP_LOGD(TAG, "led count: %d", led_count);
    effect_data_.fill(0);
    for (int idx = 0; idx < led_count; idx++) {
      state_.emplace_back(&leds_[idx].red, &leds_[idx].green, &leds_[idx].blue, nullptr, &effect_data_[idx],
                          &correction_);
    }
  }

  void loop() override {
    if (should_show_()) {
      update_leds();
    }
  }

  float get_setup_priority() const override { return setup_priority::HARDWARE; }

  int32_t size() const override { return led_count; }

  light::LightTraits get_traits() override {
    auto traits = light::LightTraits();
    traits.set_supports_rgb(true);
    traits.set_supports_brightness(true);
    return traits;
  }

 protected:
  light::ESPColorView get_view_internal(int32_t index) const override { return state_[index]; }

 private:
  void update_leds() {
    for (int index = 0; index < leds_.size(); index++) {
      uint8_t data_out[4] = {0x00};
      data_out[0] = index;
      data_out[1] = leds_[index].red;
      data_out[2] = leds_[index].green;
      data_out[3] = leds_[index].blue;
      write_bytes(led_register, data_out, 4);
    }
    mark_shown_();
  }
};
}  // namespace yunhat
}  // namespace esphome
