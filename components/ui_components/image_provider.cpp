#include "image_provider.h"

namespace esphome {

ui_components::ImageProvider::ImageProvider(display::Image *content) {
  dynamic_content_ = [this, content]() { return content; };
}
display::Image *ui_components::ImageProvider::get_image() { return dynamic_content_(); }
#if defined(USE_ICON_PROVIDER)
#if defined(USE_TEXT_SENSOR)
ui_components::ImageProvider::ImageProvider(text_sensor::TextSensor *sensor_name, icon_provider::IconProvider *icon) {}
#endif
ui_components::ImageProvider::ImageProvider(const std::string name, icon_provider::IconProvider *icon) {}
ui_components::ImageProvider::ImageProvider(const std::function<std::string()> &dynamic_name,
                                            icon_provider::IconProvider *icon) {}
#endif
}  // namespace esphome
