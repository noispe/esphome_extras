#include "inkplate.h"
#include "esphome/core/application.h"
#include "esphome/core/helpers.h"
#include "esphome/core/log.h"

#ifdef USE_ESP32_FRAMEWORK_ARDUINO

#include <esp32-hal-gpio.h>

namespace esphome {
namespace inkplate10 {
static const char *const TAG = "inkplate";

class Profile {
 public:
  Profile(const std::string &name);
  ~Profile();
  void status(const std::string &message);

 private:
  uint32_t start_time_ = 0;
  std::string name_{};
};

Profile::Profile(const std::string &name) : name_(name) {
  start_time_ = millis();
  ESP_LOGV(TAG, "%s started", name_.c_str());
}

Profile::~Profile() { ESP_LOGV(TAG, "%s finished (%ums)", name_.c_str(), millis() - start_time_); }

void Profile::status(const std::string &message) {
  ESP_LOGV(TAG, "%s at %s (%ums)", name_.c_str(), message.c_str(), millis() - start_time_);
}

#define CL_SET \
  { GPIO.out_w1ts = (1 << this->cl_pin_->get_pin()); }
#define CL_CLEAR \
  { GPIO.out_w1tc = (1 << this->cl_pin_->get_pin()); }
#define CKV_SET \
  { GPIO.out1_w1ts.val = (1 << this->ckv_pin_->get_pin()); }
#define CKV_CLEAR \
  { GPIO.out1_w1tc.val = (1 << this->ckv_pin_->get_pin()); }
#define SPH_SET \
  { GPIO.out1_w1ts.val = (1 << this->sph_pin_->get_pin()); }
#define SPH_CLEAR \
  { GPIO.out1_w1tc.val = (1 << this->sph_pin_->get_pin()); }
#define LE_SET \
  { GPIO.out_w1ts = (1 << this->le_pin_->get_pin()); }
#define LE_CLEAR \
  { GPIO.out_w1tc = (1 << this->le_pin_->get_pin()); }
#define OE_SET \
  { this->oe_pin_->digital_write(true); }
#define OE_CLEAR \
  { this->oe_pin_->digital_write(false); }
#define GMOD_SET \
  { this->gmod_pin_->digital_write(true); }
#define GMOD_CLEAR \
  { this->gmod_pin_->digital_write(false); }
#define SPV_SET \
  { this->spv_pin_->digital_write(true); }
#define SPV_CLEAR \
  { this->spv_pin_->digital_write(false); }
#define WAKEUP_SET \
  { this->wakeup_pin_->digital_write(true); }
#define WAKEUP_CLEAR \
  { this->wakeup_pin_->digital_write(false); }
#define PWRUP_SET \
  { this->powerup_pin_->digital_write(true); }
#define PWRUP_CLEAR \
  { this->powerup_pin_->digital_write(false); }
#define VCOM_SET \
  { this->vcom_pin_->digital_write(true); }
#define VCOM_CLEAR \
  { this->vcom_pin_->digital_write(true); }

void Inkplate10::setup() {
  Profile p("Inkplate10::setup");
  this->initialize_();
  this->vcom_pin_->setup();
  this->powerup_pin_->setup();
  this->wakeup_pin_->setup();
  this->gpio0_enable_pin_->setup();
  this->gpio0_enable_pin_->digital_write(true);

  WAKEUP_SET
  this->write_bytes(0x09, std::array<uint8_t, 4>{
                              0b00011011,  // Power up seq.
                              0b00000000,  // Power up delay (3mS per rail)
                              0b00011011,  // Power down seq.
                              0b00000000   // Power down delay (6mS per rail)
                          });
  WAKEUP_CLEAR

  // control pins
  this->cl_pin_->setup();
  this->le_pin_->setup();
  this->ckv_pin_->setup();
  this->gmod_pin_->setup();
  this->oe_pin_->setup();
  this->sph_pin_->setup();
  this->spv_pin_->setup();

  // data pins
  this->display_data_0_pin_->setup();
  this->display_data_1_pin_->setup();
  this->display_data_2_pin_->setup();
  this->display_data_3_pin_->setup();
  this->display_data_4_pin_->setup();
  this->display_data_5_pin_->setup();
  this->display_data_6_pin_->setup();
  this->display_data_7_pin_->setup();
}

void Inkplate10::initialize_() {
  Profile p("Inkplate10::initialize");
  uint32_t buffer_size = this->get_buffer_length_();

  if (this->partial_buffer_ != nullptr) {
    free(this->partial_buffer_);  // NOLINT
  }
  if (this->partial_buffer_2_ != nullptr) {
    free(this->partial_buffer_2_);  // NOLINT
  }
  if (this->buffer_ != nullptr) {
    free(this->buffer_);  // NOLINT
  }

  this->buffer_ = (uint8_t *) ps_malloc(buffer_size);
  if (this->buffer_ == nullptr) {
    ESP_LOGE(TAG, "Could not allocate buffer for display!");
    this->mark_failed();
    return;
  }
  if (!this->greyscale_) {
    this->partial_buffer_ = (uint8_t *) ps_malloc(buffer_size);
    if (this->partial_buffer_ == nullptr) {
      ESP_LOGE(TAG, "Could not allocate partial buffer for display!");
      this->mark_failed();
      return;
    }
    this->partial_buffer_2_ = (uint8_t *) ps_malloc(buffer_size * 2);
    if (this->partial_buffer_2_ == nullptr) {
      ESP_LOGE(TAG, "Could not allocate partial buffer 2 for display!");
      this->mark_failed();
      return;
    }
    memset(this->partial_buffer_, 0, buffer_size);
    memset(this->partial_buffer_2_, 0, buffer_size * 2);
  }

  memset(this->buffer_, 0, buffer_size);

  for (int j = 0; j < 8; ++j) {
    for (uint32_t i = 0; i < 256; ++i) {
      uint8_t z = (this->waveform3_bit_(i & 0x07, j) << 2) | (this->waveform3_bit_((i >> 4) & 0x07, j));
      glut_[j * 256 + i] = ((z & 0b00000011) << 4) | (((z & 0b00001100) >> 2) << 18) | (((z & 0b00010000) >> 4) << 23) |
                           (((z & 0b11100000) >> 5) << 25);
      z = ((this->waveform3_bit_(i & 0x07, j) << 2) | (this->waveform3_bit_((i >> 4) & 0x07, j))) << 4;
      glut2_[j * 256 + i] = ((z & 0b00000011) << 4) | (((z & 0b00001100) >> 2) << 18) |
                            (((z & 0b00010000) >> 4) << 23) | (((z & 0b11100000) >> 5) << 25);
    }
  }

  for (uint32_t i = 0; i < 256; ++i) {
    pinLUT_[i] = ((i & 0b00000011) << 4) | (((i & 0b00001100) >> 2) << 18) | (((i & 0b00010000) >> 4) << 23) |
                 (((i & 0b11100000) >> 5) << 25);
  }
}

float Inkplate10::get_setup_priority() const { return setup_priority::PROCESSOR; }
size_t Inkplate10::get_buffer_length_() {
  if (this->greyscale_) {
    return size_t(this->get_width_internal()) * size_t(this->get_height_internal()) / 2u;
  } else {
    return size_t(this->get_width_internal()) * size_t(this->get_height_internal()) / 8u;
  }
}
void Inkplate10::update() {
  Profile p("Inkplate10::update");
  this->do_update_();

  if (this->full_update_every_ > 0 && this->partial_updates_ >= this->full_update_every_) {
    this->block_partial_ = true;
  }

  this->display();
}
void HOT Inkplate10::draw_absolute_pixel_internal(int x0, int y0, Color color) {
  if (x0 >= this->get_width_internal() || y0 >= this->get_height_internal() || x0 < 0 || y0 < 0)
    return;

  uint8_t gs = ((color.red * 2126 / 10000) + (color.green * 7152 / 10000) + (color.blue * 722 / 10000)) >> 5;
  if (this->greyscale_ == false) {
    int x = x0 >> 3;
    int x_sub = x0 & 7;
    uint8_t temp = *(this->partial_buffer_ + ((this->get_width_internal() >> 3) * y0) + x);
    *(this->partial_buffer_ + (this->get_width_internal() / 8 * y0) + x) =
        (~pixelMaskLUT[x_sub] & temp) | (color.is_on() ? 0 : pixelMaskLUT[x_sub]);
  } else {
    gs &= 7;
    int x = x0 >> 1;
    int x_sub = x0 & 1;
    uint8_t temp;
    temp = *(this->buffer_ + (this->get_width_internal() >> 1) * y0 + x);
    *(this->buffer_ + (this->get_width_internal() >> 1) * y0 + x) =
        (pixelMaskGLUT[x_sub] & temp) | (x_sub ? gs : gs << 4);
  }
}

void Inkplate10::dump_config() {
  LOG_DISPLAY("", "Inkplate", this);
  ESP_LOGCONFIG(TAG, "  Greyscale: %s", YESNO(this->greyscale_));
  ESP_LOGCONFIG(TAG, "  Partial Updating: %s", YESNO(this->partial_updating_));
  ESP_LOGCONFIG(TAG, "  Full Update Every: %d", this->full_update_every_);
  // Log pins
  LOG_PIN("  CKV Pin: ", this->ckv_pin_);
  LOG_PIN("  CL Pin: ", this->cl_pin_);
  LOG_PIN("  GPIO0 Enable Pin: ", this->gpio0_enable_pin_);
  LOG_PIN("  GMOD Pin: ", this->gmod_pin_);
  LOG_PIN("  LE Pin: ", this->le_pin_);
  LOG_PIN("  OE Pin: ", this->oe_pin_);
  LOG_PIN("  POWERUP Pin: ", this->powerup_pin_);
  LOG_PIN("  SPH Pin: ", this->sph_pin_);
  LOG_PIN("  SPV Pin: ", this->spv_pin_);
  LOG_PIN("  VCOM Pin: ", this->vcom_pin_);
  LOG_PIN("  WAKEUP Pin: ", this->wakeup_pin_);

  LOG_PIN("  Data 0 Pin: ", this->display_data_0_pin_);
  LOG_PIN("  Data 1 Pin: ", this->display_data_1_pin_);
  LOG_PIN("  Data 2 Pin: ", this->display_data_2_pin_);
  LOG_PIN("  Data 3 Pin: ", this->display_data_3_pin_);
  LOG_PIN("  Data 4 Pin: ", this->display_data_4_pin_);
  LOG_PIN("  Data 5 Pin: ", this->display_data_5_pin_);
  LOG_PIN("  Data 6 Pin: ", this->display_data_6_pin_);
  LOG_PIN("  Data 7 Pin: ", this->display_data_7_pin_);

  LOG_UPDATE_INTERVAL(this);
}

void Inkplate10::eink_off_() {
  if (!panel_on_)
    return;
  Profile p("Inkplate10::eink_off_");

  OE_CLEAR;
  GMOD_CLEAR;

  GPIO.out &= ~(DATA | (1 << this->cl_pin_->get_pin()) | (1 << this->le_pin_->get_pin()));
  CKV_CLEAR;
  SPH_CLEAR;
  SPV_CLEAR;

  // Put TPS65186 into standby mode (leaving 3V3 SW active)
  VCOM_CLEAR;
  this->write_byte(0x01, 0x6F);
  delay(100);

  // Disable 3V3 to the panel
  this->write_byte(0x01, 0x4F);
  WAKEUP_CLEAR;

  pins_z_state_();
  panel_on_ = false;
}

bool Inkplate10::eink_on_() {
  if (panel_on_)
    return panel_on_;
  Profile p("Inkplate10::eink_on_");

  WAKEUP_SET;
  VCOM_SET;
  delay(2);

  // Enable all rails
  this->write_byte(0x01, 0b00101111);

  delay(1);
  // Switch TPS65186 into active mode
  this->write_byte(0x01, 0b10101111);

  pins_as_outputs_();

  LE_CLEAR;
  OE_CLEAR;
  CL_CLEAR;
  SPH_SET;
  GMOD_SET;
  SPV_SET;
  CKV_CLEAR;
  OE_CLEAR;

  unsigned long timer = millis();
  // waits for ok status for each rail
  do {
    delay(1);
  } while ((this->read_byte(0x0F) != 0b11111010) && (millis() - timer) < 250);
  if ((millis() - timer) >= 250) {
    ESP_LOGE(TAG, "Eink not ready");
    WAKEUP_CLEAR;
    VCOM_CLEAR;
    PWRUP_CLEAR;
    this->mark_failed();
    panel_on_ = false;
    return panel_on_;
  }

  OE_SET;
  panel_on_ = true;
  return panel_on_;
}

void Inkplate10::fill(Color color) {
  ESP_LOGD(TAG, "%x,%x,%x fill", color.r, color.g, color.b);
  Profile p("Inkplate10::fill");

  if (this->greyscale_) {
    uint8_t fill = ((color.red * 2126 / 10000) + (color.green * 7152 / 10000) + (color.blue * 722 / 10000)) >> 5;
    memset(this->buffer_, (fill << 4) | fill, this->get_buffer_length_());
  } else {
    uint8_t fill = color.is_on() ? 0x00 : 0xFF;
    memset(this->partial_buffer_, fill, this->get_buffer_length_());
  }
}
void Inkplate10::display() {
  Profile p("Inkplate10::display");

  if (this->greyscale_) {
    this->display3b_();
  } else {
    if (this->partial_updating_ && this->partial_update_()) {
      p.status("partial update");
      return;
    }
    this->display1b_();
  }
}

void Inkplate10::display1b_() {
  Profile p("Inkplate10::display1b_");

  memcpy(this->buffer_, this->partial_buffer_, this->get_buffer_length_());

  uint32_t CL = (1 << this->cl_pin_->get_pin());
  uint32_t pix = 0;
  uint8_t repeat = 0;

  if (!this->eink_on_())
    return;

  if (this->light_mode_) {
    this->clean_fast_(0, 1);
    this->clean_fast_(1, 12);
    this->clean_fast_(2, 1);
    this->clean_fast_(0, 9);
    this->clean_fast_(2, 1);
    this->clean_fast_(1, 12);
    this->clean_fast_(2, 1);
    this->clean_fast_(0, 9);
    repeat = 3;
  } else {
    this->clean_fast_(0, 1);
    this->clean_fast_(1, 10);
    this->clean_fast_(2, 1);
    this->clean_fast_(0, 10);
    this->clean_fast_(2, 1);
    this->clean_fast_(1, 10);
    this->clean_fast_(2, 1);
    this->clean_fast_(0, 10);
    repeat = 5;
  }

  for (int k = 0; k < repeat; k++) {
    uint32_t _pos = (this->get_height_internal() * this->get_width_internal() / 8) - 1;
    this->vscan_start_();
    for (int i = 0; i < this->get_height_internal(); i++) {
      uint8_t dram = ~(*(this->buffer_ + _pos));
      uint8_t data = LUTB[(dram >> 4) & 0x0F];
      this->hscan_start_(pinLUT_[data]);
      data = LUTB[dram & 0x0F];
      GPIO.out_w1ts = pinLUT_[data] | CL;
      GPIO.out_w1tc = DATA | CL;
      _pos--;
      for (int j = 0; j < ((this->get_width_internal() / 8) - 1); j++) {
        pix += *(this->buffer_ + _pos) > 0 ? 1 : 0;
        dram = (*(this->buffer_ + _pos));
        data = LUTB[(dram >> 4) & 0x0F];
        GPIO.out_w1ts = pinLUT_[data] | CL;
        GPIO.out_w1tc = DATA | CL;
        data = LUTB[dram & 0x0F];
        GPIO.out_w1ts = pinLUT_[data] | CL;
        GPIO.out_w1tc = DATA | CL;
        _pos--;
      }
      GPIO.out_w1ts = CL;
      GPIO.out_w1tc = DATA | CL;
      this->vscan_end_();
    }
    delayMicroseconds(230);
  }
  ESP_LOGD(TAG, "drew %d pixels", pix);

  this->clean_fast_(2, 2);
  this->clean_fast_(3, 1);

  this->vscan_start_();
  if (!this->power_control_) {
    this->eink_off_();
  }
  this->block_partial_ = false;
  this->partial_updates_ = 0;
}

void Inkplate10::display3b_() {
  Profile p("Inkplate10::display3b_");

  uint32_t CL = (1 << this->cl_pin_->get_pin());

  if (!this->eink_on_())
    return;

  if (this->light_mode_) {
    this->clean_fast_(1, 1);
    this->clean_fast_(0, 7);
    this->clean_fast_(2, 1);
    this->clean_fast_(1, 12);
    this->clean_fast_(2, 1);
    this->clean_fast_(0, 7);
    this->clean_fast_(2, 1);
    this->clean_fast_(1, 12);
  } else {
    this->clean_fast_(1, 1);
    this->clean_fast_(0, 10);
    this->clean_fast_(2, 1);
    this->clean_fast_(1, 10);
    this->clean_fast_(2, 1);
    this->clean_fast_(0, 10);
    this->clean_fast_(2, 1);
    this->clean_fast_(1, 10);
  }

  for (int k = 0; k < 8; k++) {
    uint8_t *dp = this->buffer_ + (this->get_height_internal() * this->get_width_internal() / 2);

    vscan_start_();
    for (int i = 0; i < this->get_height_internal(); i++) {
      uint32_t t = glut2_[k * 256 + (*(--dp))];
      t |= glut_[k * 256 + (*(--dp))];
      hscan_start_(t);
      t = glut2_[k * 256 + (*(--dp))];
      t |= glut_[k * 256 + (*(--dp))];
      GPIO.out_w1ts = t | CL;
      GPIO.out_w1tc = DATA | CL;

      for (int j = 0; j < ((this->get_width_internal() / 8) - 1); j++) {
        t = glut2_[k * 256 + (*(--dp))];
        t |= glut_[k * 256 + (*(--dp))];
        GPIO.out_w1ts = t | CL;
        GPIO.out_w1tc = DATA | CL;
        t = glut2_[k * 256 + (*(--dp))];
        t |= glut_[k * 256 + (*(--dp))];
        GPIO.out_w1ts = t | CL;
        GPIO.out_w1tc = DATA | CL;
      }

      GPIO.out_w1ts = CL;
      GPIO.out_w1tc = DATA | CL;
      vscan_end_();
    }
    delayMicroseconds(230);
  }
  clean_fast_(3, 1);
  vscan_start_();
  if (!this->power_control_) {
    eink_off_();
  }
}

bool Inkplate10::partial_update_() {
  Profile p("Inkplate10::partial_update_");
  if (this->greyscale_)
    return false;
  if (this->block_partial_) {
    return false;
  }

  this->partial_updates_++;

  uint32_t _pos = (this->get_width_internal() * this->get_height_internal() / 8) - 1;
  uint32_t n = (this->get_width_internal() * this->get_height_internal() / 4) - 1;
  uint8_t repeat = this->light_mode_ ? 4 : 5;
  uint32_t changeCount = 0;

  for (int i = 0; i < this->get_height_internal(); ++i) {
    for (int j = 0; j < this->get_width_internal() / 8; ++j) {
      uint8_t diffw = *(this->buffer_ + _pos) & ~*(partial_buffer_ + _pos);
      uint8_t diffb = ~*(this->buffer_ + _pos) & *(partial_buffer_ + _pos);
      if (diffw)  // count pixels turning from black to white as these are
                  // visible blur
      {
        for (int bv = 1; bv < 256; bv <<= 1) {
          if (diffw & bv)
            ++changeCount;
        }
      }
      _pos--;
      *(this->partial_buffer_2_ + n) = LUTW[diffw >> 4] & (LUTB[diffb >> 4]);
      n--;
      *(this->partial_buffer_2_ + n) = LUTW[diffw & 0x0F] & (LUTB[diffb & 0x0F]);
      n--;
    }
  }

  p.status("update buffer built");

  if (!eink_on_())
    return false;

  uint32_t CL = (1 << this->cl_pin_->get_pin());

  for (int k = 0; k < repeat; ++k) {
    vscan_start_();
    n = (this->get_width_internal() * this->get_height_internal() / 4) - 1;
    for (int i = 0; i < this->get_height_internal(); ++i) {
      uint8_t data = *(this->partial_buffer_2_ + n);
      uint8_t _send = pinLUT_[data];
      hscan_start_(_send);
      n--;
      for (int j = 0; j < ((this->get_width_internal() / 4) - 1); ++j) {
        data = *(this->partial_buffer_2_ + n);
        _send = pinLUT_[data];
        GPIO.out_w1ts = _send | CL;
        GPIO.out_w1tc = DATA | CL;
        n--;
      }
      GPIO.out_w1ts = _send | CL;
      GPIO.out_w1tc = DATA | CL;
      vscan_end_();
    }
    delayMicroseconds(230);
    p.status("Partial update loop");
  }

  clean_fast_(2, 2);
  clean_fast_(3, 1);
  vscan_start_();
  if (!this->power_control_) {
    eink_off_();
  }

  memcpy(this->buffer_, this->partial_buffer_, this->get_buffer_length_());
  ESP_LOGV(TAG, "%d pixels changed", changeCount);
  return true;
}

void Inkplate10::vscan_start_() {
  uint32_t clock = (1 << this->cl_pin_->get_pin());
  CKV_SET;
  delayMicroseconds(7);
  SPV_CLEAR;
  delayMicroseconds(10);
  CKV_CLEAR;
  delayMicroseconds(0);
  CKV_SET;
  delayMicroseconds(8);
  SPV_SET;
  delayMicroseconds(10);
  CKV_CLEAR;
  delayMicroseconds(0);
  CKV_SET;
  delayMicroseconds(18);
  CKV_CLEAR;
  delayMicroseconds(0);
  CKV_SET;
  delayMicroseconds(18);
  CKV_CLEAR;
  delayMicroseconds(0);
  CKV_SET;
}

void Inkplate10::hscan_start_(uint32_t d) {
  SPH_CLEAR;
  GPIO.out_w1ts = (d) | (1 << this->cl_pin_->get_pin());
  GPIO.out_w1tc = DATA | (1 << this->cl_pin_->get_pin());
  SPH_SET;
  CKV_SET;
}
void Inkplate10::vscan_end_() {
  CKV_CLEAR;
  LE_SET;
  LE_CLEAR;
  delayMicroseconds(0);
}

void Inkplate10::clean() {
  Profile p("Inkplate10::clean");

  eink_on_();
  clean_fast_(0, 1);   // White
  clean_fast_(0, 8);   // White to White
  clean_fast_(0, 1);   // White to Black
  clean_fast_(0, 8);   // Black to Black
  clean_fast_(2, 1);   // Black to White
  clean_fast_(1, 10);  // White to White
}

void Inkplate10::clean_fast_(uint8_t c, uint8_t rep) {
  ESP_LOGV(TAG, "Clean fast called with: (%d, %d)", c, rep);
  Profile p("Inkplate10::clean_fast_");
  uint32_t CL = (1 << this->cl_pin_->get_pin());

  eink_on_();
  uint8_t data = 0;
  if (c == 0) {
    data = 0b10101010;
  } else if (c == 1) {
    data = 0b01010101;
  } else if (c == 2) {
    data = 0b00000000;
  } else if (c == 3) {
    data = 0b11111111;
  }
  uint32_t _send = pinLUT_[data];
  for (int k = 0; k < rep; ++k) {
    vscan_start_();
    for (int i = 0; i < this->get_height_internal(); ++i) {
      hscan_start_(_send);
      GPIO.out_w1ts = (_send) | CL;
      GPIO.out_w1tc = CL;
      for (int j = 0; j < ((this->get_width_internal() / 8) - 1); ++j) {
        GPIO.out_w1ts = CL;
        GPIO.out_w1tc = CL;
        GPIO.out_w1ts = CL;
        GPIO.out_w1tc = CL;
      }
      GPIO.out_w1ts = CL;
      GPIO.out_w1tc = CL;
      vscan_end_();
    }
    delayMicroseconds(230);
  }
}

void Inkplate10::pins_z_state_() {
  this->ckv_pin_->pin_mode(gpio::FLAG_INPUT);
  this->sph_pin_->pin_mode(gpio::FLAG_INPUT);

  this->oe_pin_->pin_mode(gpio::FLAG_INPUT);
  this->gmod_pin_->pin_mode(gpio::FLAG_INPUT);
  this->spv_pin_->pin_mode(gpio::FLAG_INPUT);

  this->display_data_0_pin_->pin_mode(gpio::FLAG_INPUT);
  this->display_data_1_pin_->pin_mode(gpio::FLAG_INPUT);
  this->display_data_2_pin_->pin_mode(gpio::FLAG_INPUT);
  this->display_data_3_pin_->pin_mode(gpio::FLAG_INPUT);
  this->display_data_4_pin_->pin_mode(gpio::FLAG_INPUT);
  this->display_data_5_pin_->pin_mode(gpio::FLAG_INPUT);
  this->display_data_6_pin_->pin_mode(gpio::FLAG_INPUT);
  this->display_data_7_pin_->pin_mode(gpio::FLAG_INPUT);
}

void Inkplate10::pins_as_outputs_() {
  this->ckv_pin_->pin_mode(gpio::FLAG_OUTPUT);
  this->sph_pin_->pin_mode(gpio::FLAG_OUTPUT);

  this->oe_pin_->pin_mode(gpio::FLAG_OUTPUT);
  this->gmod_pin_->pin_mode(gpio::FLAG_OUTPUT);
  this->spv_pin_->pin_mode(gpio::FLAG_OUTPUT);

  this->display_data_0_pin_->pin_mode(gpio::FLAG_OUTPUT);
  this->display_data_1_pin_->pin_mode(gpio::FLAG_OUTPUT);
  this->display_data_2_pin_->pin_mode(gpio::FLAG_OUTPUT);
  this->display_data_3_pin_->pin_mode(gpio::FLAG_OUTPUT);
  this->display_data_4_pin_->pin_mode(gpio::FLAG_OUTPUT);
  this->display_data_5_pin_->pin_mode(gpio::FLAG_OUTPUT);
  this->display_data_6_pin_->pin_mode(gpio::FLAG_OUTPUT);
  this->display_data_7_pin_->pin_mode(gpio::FLAG_OUTPUT);
}

void Inkplate10::power_off() { this->eink_off_(); }

const uint8_t Inkplate10::waveform3_bit_(uint32_t r, uint32_t c) const {
  if (this->light_mode_) {
    return waveform3BitLight[r][c];
  }
  return waveform3Bit[r][c];
}

}  // namespace inkplate10
}  // namespace esphome

#endif
