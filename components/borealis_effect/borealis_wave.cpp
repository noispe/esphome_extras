#include "borealis_wave.h"
#include "esphome/core/log.h"

namespace esphome {
static const char *VTAG = "BorealisLightEffect";

// List of colors allowed for waves
// The first dimension of this array must match the second dimension of the colorwighting array
static constexpr uint8_t allowedcolors[5][3] = {
    {17, 177, 13},    // Greenish
    {148, 242, 5},    // Greenish
    {25, 173, 121},   // Turquoise
    {250, 77, 127},   // Pink
    {171, 101, 221},  // Purple
};

// Colorweighing allows to give some colors more weight so it is more likely to be choosen for a wave.
// The second dimension of this array must match the first dimension of the allowedcolors array
// Here are 3 presets.
static constexpr uint8_t colorweighting[3][5] = {
    {10, 10, 10, 10, 10},  // Weighting equal (every color is equally likely)
    {2, 2, 2, 6, 6},       // Weighting reddish (red colors are more likely)
    {6, 6, 6, 2, 2}        // Weighting greenish (green colors are more likely)
};

BorealisWave::BorealisWave(uint8_t num_leds, uint8_t width_factor, uint8_t color_weight_preset, uint8_t speed_factor)
    : num_leds_(num_leds),
      wave_width_factor_(width_factor),
      wave_color_weight_preset_(color_weight_preset),
      wave_speed_factor_(speed_factor) {
  width_ = random_float_(num_leds_ / 10, num_leds_ / wave_width_factor_);
  center_ = random_float_(0, 101) / 100.0f * num_leds_;
  basecolor_ = get_weighted_color_(wave_color_weight_preset_);
  speed_ = random_float_(10, 30) / 100.0f * wave_speed_factor_;
  basealpha_ = random_float_(75, 101) / 100.0f;
  ttl_ = random_int_(500, 1501);
  going_left_ = random_int_(0, 4) > 2;

  ESP_LOGD(VTAG, "width_: %f, center_: %f, basecolor_: %x, speed_: %f", width_, center_, basecolor_, speed_);
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
    float offset_factor = (float) offset / (width_ / 2.0f);

    // The age of the wave determines it brightness.
    // At half its maximum age it will be the brightest.
    float age_factor = 1;
    if ((float) age_ / ttl_ < 0.5) {
      age_factor = (float) age_ / (ttl_ / 2.0f);
    } else {
      age_factor = (float) (ttl_ - age_) / ((float) ttl_ * 0.5f);
    }

    // Calculate color based on above factors and basealpha value
    rgb.r = allowedcolors[basecolor_][0] * (1 - offset_factor) * age_factor * basealpha_;
    rgb.g = allowedcolors[basecolor_][1] * (1 - offset_factor) * age_factor * basealpha_;
    rgb.b = allowedcolors[basecolor_][2] * (1 - offset_factor) * age_factor * basealpha_;

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
  auto sum_of_weights = 0;

  for (auto i = 0; i < sizeof colorweighting[0]; i++) {
    sum_of_weights += colorweighting[weighting][i];
  }

  auto randomweight = random_int_(0, sum_of_weights);

  for (auto i = 0; i < sizeof colorweighting[0]; i++) {
    if (randomweight < colorweighting[weighting][i]) {
      return i;
    }

    randomweight -= colorweighting[weighting][i];
  }
  return 0;
}

int BorealisWave::random_int_(int min, int max) { return random_uint32() % (max - min + 1) + min; }

float BorealisWave::random_float_(float min, float max) {
  return (double) random_uint32() / UINT32_MAX * (max - min) + min;
}

void borealis::BorealisLightEffect::blank_all_leds_(light::AddressableLight &it) {
  for (int led = it.size(); led-- > 0;) {
    it[led].set(Color::BLACK);
  }
  it.schedule_show();
}

borealis::BorealisLightEffect::BorealisLightEffect(const std::string &name) : AddressableLightEffect(name) {}

void borealis::BorealisLightEffect::start() {
  wave_ = std::make_shared<BorealisWave>(num_leds_, width_factor_, color_weight_preset_, speed_factor_);
}

void borealis::BorealisLightEffect::stop() { wave_.reset(); }

void borealis::BorealisLightEffect::apply(light::AddressableLight &it, const Color &current_color) {
  wave_->update();
  for (int led = it.size(); led-- > 0;) {
    it[led].set(wave_->get_color_for_led(led).value_or(Color::BLACK));
  }
  if (!wave_->still_alive()) {
    wave_ = std::make_shared<BorealisWave>(num_leds_, width_factor_, color_weight_preset_, speed_factor_);
  }
  it.schedule_show();
}
}  // namespace esphome
