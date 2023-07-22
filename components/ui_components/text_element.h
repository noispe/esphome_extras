#pragma once

#include "elements.h"

#ifdef USE_TEXT_SENSOR
#include "esphome/components/text_sensor/text_sensor.h"
#endif
#ifdef USE_SENSOR
#include "esphome/components/sensor/sensor.h"
#endif
#ifdef USE_BINARY_SENSOR
#include "esphome/components/binary_sensor/binary_sensor.h"
#endif
#ifdef USE_ICON_PROVIDER
#include "esphome/components/icons/icon_provider.h"
#endif
#ifdef USE_SELECT
#include "esphome/components/select/select.h"
#endif
#ifdef USE_NUMBER
#include "esphome/components/number/number.h"
#endif

namespace esphome {
namespace ui_components {
class TextElement : public BaseElement {
 public:
  void draw(display::Display &disp) override;
  void set_font(font::Font *font) { font_ = font; }
#ifdef USE_SENSOR
  void set_content(sensor::Sensor *content);
#endif
#ifdef USE_BINARY_SENSOR
  void set_content(binary_sensor::BinarySensor *content);
#endif
#ifdef USE_TEXT_SENSOR
  void set_content(text_sensor::TextSensor *content);
#endif
#ifdef USE_SELECT
  void set_content(select::Select *content);
#endif
#ifdef USE_NUMBER
  void set_content(number::Number *content);
#endif
  void set_content(const std::string &content);
  void set_content(const std::function<std::string()> &content) { dynamic_content_ = content; }
  void set_default(const std::string &d) { default_ = d; }
  font::Font *get_font() const { return font_ == nullptr ? parent_->default_font() : font_; }
 private:
  font::Font *font_ = nullptr;
  std::function<std::string()> dynamic_content_{};
  std::string default_;
};
}  // namespace ui_components
}  // namespace esphome
