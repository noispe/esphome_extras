#pragma once

#include "esphome/core/component.h"
#include "esphome/core/color.h"
#include "esphome/components/light/addressable_light_effect.h"

namespace esphome {

class BorealisWave {
 public:
  BorealisWave() = default;
  BorealisWave(uint8_t num_leds, uint8_t width_factor, uint8_t color_weight_preset = 1, uint8_t speed_factor = 3);
  optional<Color> get_color_for_led(int index) const;
  void update();
  bool still_alive() const;

 private:
  int random_int_(int min, int max);
  float random_float_(float min, float max);
  uint8_t get_weighted_color_(uint8_t weighting);

  int ttl_ = 1;
  uint8_t basecolor_ = 1;  // 1
  float basealpha_ = 1;
  int age_ = 0;
  float width_ = 1.0f;  // 1
  float center_ = 1;    // 1
  bool going_left_ = true;
  float speed_ = 1;  // 1
  bool alive_ = true;
  uint8_t num_leds_ = 1;                  // Number of LEDS on strip
  uint8_t wave_width_factor_ = 1;         // Higher number, smaller waves
  uint8_t wave_color_weight_preset_ = 1;  // What color weighting to choose
  uint8_t wave_speed_factor_ = 1;         // Higher number, higher speed
};

namespace borealis {

class BorealisLightEffect : public light::AddressableLightEffect {
 public:
  BorealisLightEffect(const std::string &name);

 public:
  void start() override;
  void stop() override;
  void apply(light::AddressableLight &it, const Color &current_color) override;

  void set_width_factor(const uint8_t &width_factor) { width_factor_ = width_factor; }
  void set_color_weight_preset(const uint8_t &color_weight_preset) { color_weight_preset_ = color_weight_preset; }
  void set_speed_factor(const uint8_t &speed_factor) { speed_factor_ = speed_factor; }
  void set_num_leds(const uint8_t &num_leds) { num_leds_ = num_leds; }

 private:
  void blank_all_leds_(light::AddressableLight &it);

  uint8_t num_leds_ = 1;  // Number of LEDS on strip
  uint8_t width_factor_;
  uint8_t color_weight_preset_;
  uint8_t speed_factor_;
  std::shared_ptr<BorealisWave> wave_ = nullptr;
};

}  // namespace borealis
}  // namespace esphome
