#include "button_elements.h"
#include "esphome/core/log.h"

namespace esphome {
namespace ui_components {
static const char *const TAG = "button_element";
}
#ifdef USE_TOUCHSCREEN

void ui_components::AbstractButton::set_touchscreen(touchscreen::Touchscreen *touchscreen) {
  touchscreen->register_listener(this);
}
void ui_components::AbstractButton::set_control(binary_sensor::BinarySensor *sensor) { proxy_ = sensor; }

void ui_components::AbstractButton::touch(touchscreen::TouchPoint tp) {
  if (this->in_range(tp.x, tp.y)) {
    proxy_->publish_state(true);
  }
}

void ui_components::AbstractButton::release() {
  if (is_pressed()) {
    proxy_->publish_state(false);
  }
}

bool ui_components::AbstractButton::is_pressed() { return proxy_->state; }

void ui_components::TextButtonElement::draw(display::DisplayBuffer &disp) {
  /*
  auto old_bg_color = get_bg_color();
  auto old_fg_color = get_fg_color();
  get_bg_color() = this->is_pressed() ? get_fg_color() : get_bg_color();
  get_fg_color() = this->is_pressed() ? get_bg_color() : get_fg_color();
  */
  ESP_LOGD(TAG, "Draw button at %d,%d - %dx%d", x_, y_, width_, height_);
  Primitives::filled_round_rect(disp, x_, y_, width_, height_, radius_, get_bg_color());
  Primitives::round_rect(disp, x_, y_, width_, height_, radius_, get_fg_color());
  TextElement::draw(disp);
  /*
  get_bg_color() = old_bg_color;
  get_fg_color() = old_fg_color;
  */
}

void ui_components::IconButtonElement::draw(display::DisplayBuffer &disp) {
  auto old_bg_color = get_bg_color();
  auto old_fg_color = get_fg_color();
  get_bg_color() = this->is_pressed() ? get_fg_color() : get_bg_color();
  get_fg_color() = this->is_pressed() ? get_bg_color() : get_fg_color();
  ESP_LOGD(TAG, "Draw button at %d,%d - %dx%d", x_, y_, width_, height_);
  Primitives::filled_round_rect(disp, x_, y_, width_, height_, radius_, get_bg_color());
  Primitives::round_rect(disp, x_, y_, width_, height_, radius_, get_fg_color());
  ImageElement::draw(disp);
  get_bg_color() = old_bg_color;
  get_fg_color() = old_fg_color;
}

bool ui_components::TextButtonElement::in_range(uint16_t x, uint16_t y) const {
  if (x < this->x()) {
    return false;
  }
  if (x > this->x() + this->width()) {
    return false;
  }
  if (y < this->y()) {
    return false;
  }
  if (y > this->y() + this->height()) {
    return false;
  }
  return true;
}

bool ui_components::IconButtonElement::in_range(uint16_t x, uint16_t y) const {
  if (x < this->x()) {
    return false;
  }
  if (x > this->x() + this->width()) {
    return false;
  }
  if (y < this->y()) {
    return false;
  }
  if (y > this->y() + this->height()) {
    return false;
  }
  return true;
}

int ui_components::TextButtonElement::margin() const { return radius_/2; }

int ui_components::IconButtonElement::margin() const { return radius_/2; }

#endif

}  // namespace esphome
