#pragma once

#include "esphome/core/component.h"
#include "esphome/components/display/display_buffer.h"

#include <string>
#include <map>

namespace esphome {
namespace icon_provider {

struct Icon {
    std::string name;
    size_t width;
    size_t height;
    const uint8_t* data;
  };

class IconProvider : public Component {
 public:

  IconProvider(std::initializer_list<Icon> lut);
  display::Image *get_icon(const std::string &name);

  void dump_config() override;
  float get_setup_priority() const override;

 private:
  std::map<std::string, display::Image> lut_;
  const uint8_t empty_data_[10] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};
  display::Image empty_ {empty_data_,1,1, esphome::display::ImageType::IMAGE_TYPE_BINARY};
};

}  // namespace icon_provider
}  // namespace esphome
