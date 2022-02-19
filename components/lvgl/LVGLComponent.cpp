#include "LVGLComponent.h"
#include "esphome/core/color.h"
#include "esphome/core/log.h"

void esphome::lvgl::LVGLComponent::setup() {
  lv_init();
  //  lv_log_register_print_cb(my_log_cb);

  lv_disp_draw_buf_init(&this->disp_buf_, this->buf_, NULL, buf_pix_count);

  lv_disp_drv_init(&this->disp_drv_);
  this->disp_drv_.hor_res = LV_HOR_RES_MAX;
  this->disp_drv_.ver_res = LV_VER_RES_MAX;
  this->disp_drv_.flush_cb = my_gui_flush_cb;
  this->disp_drv_.draw_buf = &this->disp_buf_;
  this->disp_drv_.user_data = this;
  this->disp_drv_.direct_mode = 1;
  lv_disp_drv_register(&this->disp_drv_);

  /*Initialize the input device driver*/
  lv_indev_drv_init(&this->touch_indev_drv_);
  this->touch_indev_drv_.type = LV_INDEV_TYPE_POINTER;
  this->touch_indev_drv_.read_cb = my_touchpad_read_cb;
  this->touch_indev_drv_.user_data = this;
  lv_indev_drv_register(&this->touch_indev_drv_);
  lv_indev_drv_init(&this->button_indev_drv_);
  this->button_indev_drv_.type = LV_INDEV_TYPE_BUTTON;
  this->button_indev_drv_.read_cb = my_button_read_cb;
  this->button_indev_drv_.user_data = this;
  lv_indev_drv_register(&this->button_indev_drv_);
  this->high_freq_.start();  // avoid 16 ms delay
}

void IRAM_ATTR esphome::lvgl::LVGLComponent::loop() {
  lv_timer_handler();  // called by dispatch_loop
}

void IRAM_ATTR esphome::lvgl::LVGLComponent::my_touchpad_read_cb(lv_indev_drv_t *indev_driver, lv_indev_data_t *data) {
  LVGLComponent *dis = (LVGLComponent *) indev_driver->user_data;
  if (dis->touches_.empty()) {
    return;
  }
  touchscreen::TouchPoint tp;
  bool pressed;
  std::tie(pressed, tp) = dis->touches_.front();
  dis->touches_.pop_front();
  if (pressed) {
    data->state = LV_INDEV_STATE_PR;
    data->point.x = tp.x;
    data->point.y = tp.y;
  } else {
    data->state = LV_INDEV_STATE_REL;
  }
  data->continue_reading = !dis->touches_.empty();
}

void IRAM_ATTR esphome::lvgl::LVGLComponent::my_gui_flush_cb(lv_disp_drv_t *disp, const lv_area_t *area,
                                                             lv_color_t *color_p) {
  LVGLComponent *dis = (LVGLComponent *) disp->user_data;
  int idx = 0;
  if (dis->display_ != nullptr) {
    for (int x_idx = area->x1; x_idx < area->x2; x_idx++) {
      for (int y_idx = area->y1; x_idx < area->y2; y_idx++) {
        dis->display_->draw_pixel_at(x_idx, y_idx, Color(color_p[idx++].full));
      }
    }
  }

  lv_disp_flush_ready(disp);
}

void IRAM_ATTR esphome::lvgl::LVGLComponent::my_button_read_cb(lv_indev_drv_t *indev_driver, lv_indev_data_t *data) {
  LVGLComponent *dis = (LVGLComponent *) indev_driver->user_data;
  int btn_pr = 0;      // Get the ID (0,1,2...) of the pressed button
  bool state = false;  // get the press state
  if (state) {
    data->state = LV_INDEV_STATE_PRESSED;
  } else {
    data->state = LV_INDEV_STATE_RELEASED;
  }

  data->continue_reading = false;  // check for each button
}

void IRAM_ATTR esphome::lvgl::LVGLComponent::my_log_cb(const char *buffer) { ESP_LOGD("LVGL", buffer); }

void esphome::lvgl::LVGLComponent::set_touchscreen(touchscreen::Touchscreen *touchscreen) {
  touchscreen->register_listener(new LVGLTouchListener(this));
}

esphome::lvgl::LVGLComponent::LVGLTouchListener::LVGLTouchListener(LVGLComponent *parent)
    : touchscreen::TouchListener(), parent_(parent) {}

void esphome::lvgl::LVGLComponent::LVGLTouchListener::touch(touchscreen::TouchPoint tp) {
  parent_->touches_.push_back(std::make_tuple(true, tp));
}

void esphome::lvgl::LVGLComponent::LVGLTouchListener::release() {
  parent_->touches_.push_back(std::make_tuple(false, touchscreen::TouchPoint{}));
}
