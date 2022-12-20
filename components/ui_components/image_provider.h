#pragma once

#include "elements.h"

#include "esphome/components/display/display_buffer.h"
#ifdef USE_TEXT_SENSOR
#include "esphome/components/text_sensor/text_sensor.h"
#endif
#ifdef USE_ICON_PROVIDER
#include "esphome/components/icons/icon_provider.h"
#endif

namespace esphome {
namespace ui_components {

class ImageProvider {
 public:
  ImageProvider(display::Image *image);
  ImageProvider(const std::function<display::Image *()> &content) { dynamic_content_ = content; }
#if defined(USE_ICON_PROVIDER)
#if defined(USE_TEXT_SENSOR)
  ImageProvider(text_sensor::TextSensor *sensor_name, icon_provider::IconProvider *icon);
#endif
  ImageProvider(const std::string name, icon_provider::IconProvider *icon);
  ImageProvider(const std::function<std::string()> &dynamic_name, icon_provider::IconProvider *icon);
#endif
  display::Image *get_image();

 private:
  std::function<display::Image *()> dynamic_content_{};
};

}  // namespace ui_components
}  // namespace esphome
