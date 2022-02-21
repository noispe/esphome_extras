#pragma once

#include "esphome/core/component.h"
#include "esphome/components/display/display_buffer.h"

#include <string>
#include <unordered_map>

namespace esphome {
namespace icon_provider {

class IconProvider : public Component {
 public:
  IconProvider(display::Font *font, const std::unordered_map<std::string, const char *> lut);
  display::Font *get_font() const;
  const char *get_icon(const std::string &name) const;

  void dump_config() override;
  float get_setup_priority() const override;

 private:
  const std::unordered_map<std::string, const char *> lut_;
  display::Font *font_ = nullptr;
  const char *empty = "";
};

}  // namespace icon_provider
}  // namespace esphome
