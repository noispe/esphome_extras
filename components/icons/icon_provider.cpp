#include "icon_provider.h"
#include "esphome/core/log.h"

namespace esphome {
namespace icon_provider {
static const char *TAG = "icon_provider";

IconProvider::IconProvider(std::initializer_list<Icon> lut)
    : Component()
    {
      for(auto icon : lut) {
        lut_.emplace(icon.name, display::Image(icon.data, icon.width, icon.height, display::ImageType::IMAGE_TYPE_BINARY));
      }
    }

float IconProvider::get_setup_priority() const { return setup_priority::LATE; }

void IconProvider::dump_config() {
  ESP_LOGCONFIG(TAG, "Configuration:");
  ESP_LOGCONFIG(TAG, "   %d icons provided:", lut_.size());
  for(auto itm = lut_.begin(); itm != lut_.end(); itm++) {
    ESP_LOGCONFIG(TAG, "   '%s' %dx%d", itm->first.c_str(), itm->second.get_width(), itm->second.get_height());
  }
}

display::Image *IconProvider::get_icon(const std::string &name) {
  auto needle = lut_.find(name);
  if (needle != lut_.end()) {
    return &needle->second; // fix in esphome
  }
  ESP_LOGE(TAG, "Icon '%s' was not found",name.c_str());

  return &empty_;
}
}  // namespace icon_provider
}  // namespace esphome
