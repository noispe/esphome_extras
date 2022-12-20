#include "image_element.h"
#include "esphome/core/log.h"

namespace esphome {
static const char *const TAG = "image_element";

void ui_components::ImageElement::draw(display::DisplayBuffer &disp) {
  if (border_) {
    disp.rectangle(x_, y_, width_, height_, get_fg_color());
  }
  disp.filled_rectangle(x(), y(), width(), height(), get_bg_color());
  display::Image *img = iprovider_->get_image();
  if (img == nullptr) {
    return;
  }
  int image_x = x();
  int image_y = y();
  int image_width = img->get_width();
  int image_height = img->get_height();
  position_inside(&image_x, &image_y, image_width, image_height);
  ESP_LOGD(TAG, "Draw shape at %d,%d - %dx%d", x(), y(), width(), height());
  ESP_LOGD(TAG, "Draw image at %d,%d - %dx%d", image_x, image_y, image_width, image_height);
  disp.image(image_x, image_y, img, get_fg_color(), get_bg_color());
}
}  // namespace esphome
