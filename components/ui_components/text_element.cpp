#include "text_element.h"
#include "esphome/core/log.h"
#include <sstream>
#include <iomanip>

namespace esphome {
namespace ui_components {
static const char *const TAG = "text_element";
}
void ui_components::TextElement::draw(display::Display &disp) {
  if (!visible_) {
    return;
  }

  if (border_) {
    disp.rectangle(x_, y_, width_, height_, get_fg_color());
    disp.filled_rectangle(x(), y(), width(), height(), get_bg_color());
  }
  const std::string buffer = dynamic_content_();
  if (buffer.empty()) {
    return;
  }
  int text_x = x();
  int text_y = y();
  int text_width = 0;
  int text_height = 0;
  disp.get_text_bounds(x(), y(), buffer.c_str(), get_font(), display::TextAlign::TOP_LEFT, &text_y, &text_y, &text_width,
                       &text_height);
  position_inside(&text_x, &text_y, text_width, text_height);

  ESP_LOGD(TAG, "Draw shape at %d,%d - %dx%d", x(), y(), width(), height());
  ESP_LOGD(TAG, "Draw text at %d,%d - %dx%d", text_x, text_y, text_width, text_height);

  disp.print(text_x, text_y, get_font(), get_fg_color(), display::TextAlign::TOP_LEFT, buffer.c_str());
}

#ifdef USE_SENSOR
void ui_components::TextElement::set_content(sensor::Sensor *content) {
  set_content([this, content]() {
    if (content->has_state()) {
      std::ostringstream ss;
      ss << std::fixed << std::setprecision(content->get_accuracy_decimals()) << content->get_state()
         << content->get_unit_of_measurement();
      return ss.str();
    }
    return this->default_;
  });
}
#endif
#ifdef USE_TEXT_SENSOR
void ui_components::TextElement::set_content(text_sensor::TextSensor *content) {
  set_content([this, content]() { return content->has_state() ? content->get_state() : this->default_; });
}
#endif
#ifdef USE_BINARY_SENSOR
void ui_components::TextElement::set_content(binary_sensor::BinarySensor *content) {
  set_content([this, content]() {
    if (content->has_state()) {
      return content->state ? std::string{"ON"} : std::string{"OFF"};
    }
    return this->default_;
  });
}
#endif
#ifdef USE_SELECT
void ui_components::TextElement::set_content(select::Select *content) {
  set_content([this, content]() { return content->has_state() ? content->state : this->default_; });
}
#endif
#ifdef USE_NUMBER
void ui_components::TextElement::set_content(number::Number *content) {
  set_content([this, content]() { return content->has_state() ? std::to_string(content->state) : this->default_; });
}
#endif

void ui_components::TextElement::set_content(const std::string &content) {
  set_content([content]() { return content; });
}

}  // namespace esphome
