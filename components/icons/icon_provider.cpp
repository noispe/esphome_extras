#include "icon_provider.h"
#include "esphome/core/log.h"

namespace esphome {
namespace icon_provider {
static const char *TAG = "icon_provider";

IconProvider::IconProvider(display::Font *font, const std::unordered_map<std::string, const char *> lut)
    : Component(), lut_(lut), font_(font){}

float IconProvider::get_setup_priority() const { return setup_priority::DATA; }

void IconProvider::dump_config() {
  ESP_LOGCONFIG(TAG, "Configuration:");
  ESP_LOGCONFIG(TAG, "   %d icons provided:", lut_.size());
  for(const auto &itm : lut_) {
    ESP_LOGCONFIG(TAG, "   '%s'", itm.first.c_str());
  }
}

display::Font *IconProvider::get_font() const { return font_; }

const char *IconProvider::get_icon(const std::string &name) const {
  auto needle = lut_.find(name);
  if (needle != lut_.end()) {
    return needle->second;
  }
  ESP_LOGE(TAG, "Icon '%s' was not found",name.c_str());
for(const auto &itm : lut_) {
    ESP_LOGE(TAG, "  got '%s'", itm.first.c_str());
  }
  return empty;
}
}  // namespace icon_provider
}  // namespace esphome
