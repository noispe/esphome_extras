#include "borealis_wave.h"

namespace esphome {
BorealisWave::BorealisWave(uint8_t num_leds, uint8_t width_factor, uint8_t color_weight_preset, uint8_t speed_factor)
    : num_leds_(num_leds),
      wave_width_factor_(width_factor),
      wave_color_weight_preset_(color_weight_preset),
      wave_speed_factor_(speed_factor) {
  width_ = random_float_(num_leds_ / 10, num_leds_ / wave_width_factor_);
  center_ = random_float_(0, 101) / 100.0f * num_leds_;
  basecolor_ = get_weighted_color_(wave_color_weight_preset_);
  speed_ = random_float_(10, 30) / 100.0f * wave_speed_factor_;
}

optional<Color> BorealisWave::get_color_for_led(int index) const {
  if (index < (center_ - width_ / 2) || index > (center_ + width_ / 2)) {
    // Position out of range of this wave
    return optional<Color>{};
  } else {
    Color rgb;

    // Offset of this led from center of wave
    // The further away from the center, the dimmer the LED
    int offset = abs(index - center_);
    float offsetFactor = (float) offset / (width_ / 2);

    // The age of the wave determines it brightness.
    // At half its maximum age it will be the brightest.
    float ageFactor = 1;
    if ((float) age_ / ttl_ < 0.5) {
      ageFactor = (float) age_ / (ttl_ / 2);
    } else {
      ageFactor = (float) (ttl_ - age_) / ((float) ttl_ * 0.5);
    }

    // Calculate color based on above factors and basealpha value
    rgb.r = allowedcolors[basecolor_][0] * (1 - offsetFactor) * ageFactor * basealpha_;
    rgb.g = allowedcolors[basecolor_][1] * (1 - offsetFactor) * ageFactor * basealpha_;
    rgb.b = allowedcolors[basecolor_][2] * (1 - offsetFactor) * ageFactor * basealpha_;

    return rgb;
  }
};

// Change position and age of wave
// Determine if its sill "alive"
void BorealisWave::update() {
  if (going_left_) {
    center_ -= speed_;
  } else {
    center_ += speed_;
  }

  age_++;

  if (age_ > ttl_) {
    alive_ = false;
  } else {
    if (going_left_) {
      if (center_ + width_ / 2 < 0) {
        alive_ = false;
      }
    } else {
      if (center_ - width_ / 2 > num_leds_) {
        alive_ = false;
      }
    }
  }
};

bool BorealisWave::still_alive() const { return alive_; };

// Function to get the color for a wave based on the weighting.
// Paramter weighting: First index of colorweighting array. Basically what preset to choose.
uint8_t BorealisWave::get_weighted_color_(uint8_t weighting) {
  auto sumOfWeights = 0;

  for (auto i = 0; i < sizeof colorweighting[0]; i++) {
    sumOfWeights += colorweighting[weighting][i];
  }

  auto randomweight = random_int_(0, sumOfWeights);

  for (auto i = 0; i < sizeof colorweighting[0]; i++) {
    if (randomweight < colorweighting[weighting][i]) {
      return i;
    }

    randomweight -= colorweighting[weighting][i];
  }
  return 0;
}

int BorealisWave::random_int_(int min, int max) {
    int base =  random_double();
    return min + base % (( max + 1 ) - min);
}

float BorealisWave::random_float_(float min, float max) {
    float base =  random_float();
    return min + base * ( max - min);
}
}  // namespace esphome
