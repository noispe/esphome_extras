#pragma once

#include "esphome/core/component.h"
#include "esphome/components/binary_sensor/binary_sensor.h"
#include "esphome/components/touchscreen/touchscreen.h"
#include "esphome/components/display/display_buffer.h"
#include "lvgl.h"

#include <deque>
#include <tuple>
namespace esphome {
namespace lvgl {
class LVGLComponent : public Component {
 public:
  float get_setup_priority() const override { return setup_priority::DATA; };
  void setup() override;
  void loop() override;

  void set_touchscreen(touchscreen::Touchscreen *touchscreen);
  void set_display(display::DisplayBuffer *display) { display_ = display;}

  class LVGLTouchListener : public touchscreen::TouchListener {
   public:
    LVGLTouchListener(LVGLComponent *parent);
    void touch(touchscreen::TouchPoint tp) override;
    void release() override;

   private:
    LVGLComponent *parent_;
  };

 private:
  HighFrequencyLoopRequester high_freq_;
  display::DisplayBuffer *display_ = nullptr;
  std::deque<std::tuple<bool, touchscreen::TouchPoint>> touches_;
  static const size_t buf_pix_count = LV_HOR_RES_MAX * LV_VER_RES_MAX / 5;
  lv_disp_draw_buf_t disp_buf_;
  lv_color_t buf_[buf_pix_count];
  lv_disp_drv_t disp_drv_;
  lv_indev_drv_t touch_indev_drv_;
  lv_indev_drv_t button_indev_drv_;
  /* LVGL callbacks - Needs to be accessible from C library */
  static void IRAM_ATTR my_touchpad_read_cb(lv_indev_drv_t *indev_driver, lv_indev_data_t *data);
  static void IRAM_ATTR my_button_read_cb(lv_indev_drv_t *indev_driver, lv_indev_data_t *data);
  static void IRAM_ATTR my_gui_flush_cb(lv_disp_drv_t *disp, const lv_area_t *area, lv_color_t *color_p);
  static void IRAM_ATTR my_log_cb(const char *buffer);
};
}  // namespace lvgl
}  // namespace esphome
