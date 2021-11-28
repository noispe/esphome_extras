#pragma once

#include "esphome/components/display/display_buffer.h"
#include "esphome/components/i2c/i2c.h"
#include "esphome/core/component.h"
#include "esphome/core/hal.h"

#ifdef USE_ESP32_FRAMEWORK_ARDUINO

namespace esphome {
namespace inkplate10 {

class Inkplate10 : public PollingComponent, public display::DisplayBuffer, public i2c::I2CDevice {
 public:
  const uint32_t DATA = 0x0E8C0030;

  const uint8_t LUT2[16] = {0xAA, 0xA9, 0xA6, 0xA5, 0x9A, 0x99, 0x96, 0x95,
                            0x6A, 0x69, 0x66, 0x65, 0x5A, 0x59, 0x56, 0x55};
  const uint8_t LUTW[16] = {0xFF, 0xFE, 0xFB, 0xFA, 0xEF, 0xEE, 0xEB, 0xEA,
                            0xBF, 0xBE, 0xBB, 0xBA, 0xAF, 0xAE, 0xAB, 0xAA};
  const uint8_t LUTB[16] = {0xFF, 0xFD, 0xF7, 0xF5, 0xDF, 0xDD, 0xD7, 0xD5,
                            0x7F, 0x7D, 0x77, 0x75, 0x5F, 0x5D, 0x57, 0x55};

  const uint8_t pixelMaskLUT[8] = {0x1, 0x2, 0x4, 0x8, 0x10, 0x20, 0x40, 0x80};
  const uint8_t pixelMaskGLUT[2] = {0xF, 0xF0};

  const uint8_t discharge[16] = {0xFF, 0xFC, 0xF3, 0xF0, 0xCF, 0xCC, 0xC3, 0xC0,
                                 0x3F, 0x3C, 0x33, 0x30, 0xF,  0xC,  0x3,  0x0};
  const uint8_t waveform3Bit[8][8] = {{0, 0, 0, 0, 0, 0, 1, 0}, {0, 0, 2, 2, 2, 1, 1, 0}, {0, 2, 1, 1, 2, 2, 1, 0},
                                      {1, 2, 2, 1, 2, 2, 1, 0}, {0, 2, 1, 2, 2, 2, 1, 0}, {2, 2, 2, 2, 2, 2, 1, 0},
                                      {0, 0, 0, 0, 2, 1, 2, 0}, {0, 0, 2, 2, 2, 2, 2, 0}};
  const uint8_t waveform3BitLight[8][8] = {{0, 0, 0, 0, 0, 0, 0, 0}, {0, 0, 2, 2, 2, 1, 1, 0}, {0, 0, 2, 2, 1, 2, 1, 0},
                                           {0, 2, 2, 1, 2, 2, 1, 0}, {0, 0, 2, 1, 1, 1, 2, 0}, {0, 2, 2, 2, 1, 1, 2, 0},
                                           {0, 0, 0, 0, 1, 2, 2, 0}, {0, 0, 0, 2, 2, 2, 2, 0}};

  void set_greyscale(bool greyscale) {
    this->greyscale_ = greyscale;
    this->initialize_();
    this->block_partial_ = true;
  }
  void set_partial_updating(bool partial_updating) { this->partial_updating_ = partial_updating; }
  void set_full_update_every(uint32_t full_update_every) { this->full_update_every_ = full_update_every; }
  void set_power_control(bool enable) { this->power_control_ = enable; }

  void set_display_data_0_pin(InternalGPIOPin *data) { this->display_data_0_pin_ = data; }
  void set_display_data_1_pin(InternalGPIOPin *data) { this->display_data_1_pin_ = data; }
  void set_display_data_2_pin(InternalGPIOPin *data) { this->display_data_2_pin_ = data; }
  void set_display_data_3_pin(InternalGPIOPin *data) { this->display_data_3_pin_ = data; }
  void set_display_data_4_pin(InternalGPIOPin *data) { this->display_data_4_pin_ = data; }
  void set_display_data_5_pin(InternalGPIOPin *data) { this->display_data_5_pin_ = data; }
  void set_display_data_6_pin(InternalGPIOPin *data) { this->display_data_6_pin_ = data; }
  void set_display_data_7_pin(InternalGPIOPin *data) { this->display_data_7_pin_ = data; }

  void set_ckv_pin(InternalGPIOPin *ckv) { this->ckv_pin_ = ckv; }
  void set_cl_pin(InternalGPIOPin *cl) { this->cl_pin_ = cl; }
  void set_gpio0_enable_pin(GPIOPin *gpio0_enable) { this->gpio0_enable_pin_ = gpio0_enable; }
  void set_gmod_pin(GPIOPin *gmod) { this->gmod_pin_ = gmod; }
  void set_le_pin(InternalGPIOPin *le) { this->le_pin_ = le; }
  void set_oe_pin(GPIOPin *oe) { this->oe_pin_ = oe; }
  void set_powerup_pin(GPIOPin *powerup) { this->powerup_pin_ = powerup; }
  void set_sph_pin(InternalGPIOPin *sph) { this->sph_pin_ = sph; }
  void set_spv_pin(GPIOPin *spv) { this->spv_pin_ = spv; }
  void set_vcom_pin(GPIOPin *vcom) { this->vcom_pin_ = vcom; }
  void set_wakeup_pin(GPIOPin *wakeup) { this->wakeup_pin_ = wakeup; }

  float get_setup_priority() const override;

  void dump_config() override;

  void power_off();
  void display();
  void clean();
  void fill(Color color) override;

  void update() override;

  void setup() override;

  uint8_t get_panel_state() { return this->panel_on_; }
  bool get_greyscale() { return this->greyscale_; }
  bool get_partial_updating() { return this->partial_updating_; }

  void poweron();
  void poweroff();

 protected:
  void draw_absolute_pixel_internal(int x, int y, Color color) override;
  void display1b_();
  void display3b_();
  void initialize_();
  bool partial_update_();
  void clean_fast_(uint8_t c, uint8_t rep);

  void hscan_start_(uint32_t d);
  void vscan_end_();
  void vscan_start_();
  void vscan_write_();

  void eink_off_();
  bool eink_on_();

  void setup_pins_();
  void pins_z_state_();
  void pins_as_outputs_();

  int get_width_internal() override { return 1200; }

  int get_height_internal() override { return 825; }

  size_t get_buffer_length_();
  const uint8_t waveform3_bit_(uint32_t r, uint32_t c) const;
  std::array<uint32_t, 256 * 8> glut_{};
  std::array<uint32_t, 256 * 8> glut2_{};
  std::array<uint32_t, 256> pinLUT_{};

  bool panel_on_ = false;

  uint8_t *partial_buffer_{nullptr};
  uint8_t *partial_buffer_2_{nullptr};

  uint32_t full_update_every_{0};
  uint32_t partial_updates_{0};

  bool block_partial_ = false;
  bool greyscale_ = false;
  bool partial_updating_ = false;
  bool power_control_ = false;
  bool light_mode_ = false;

  InternalGPIOPin *display_data_0_pin_;
  InternalGPIOPin *display_data_1_pin_;
  InternalGPIOPin *display_data_2_pin_;
  InternalGPIOPin *display_data_3_pin_;
  InternalGPIOPin *display_data_4_pin_;
  InternalGPIOPin *display_data_5_pin_;
  InternalGPIOPin *display_data_6_pin_;
  InternalGPIOPin *display_data_7_pin_;

  InternalGPIOPin *ckv_pin_;
  InternalGPIOPin *cl_pin_;
  GPIOPin *gpio0_enable_pin_;
  GPIOPin *gmod_pin_;
  InternalGPIOPin *le_pin_;
  GPIOPin *oe_pin_;
  GPIOPin *powerup_pin_;
  InternalGPIOPin *sph_pin_;
  GPIOPin *spv_pin_;
  GPIOPin *vcom_pin_;
  GPIOPin *wakeup_pin_;
};

}  // namespace inkplate10
}  // namespace esphome

#endif  // USE_ESP32_FRAMEWORK_ARDUINO
