#pragma once

#include "image_provider.h"
#include "elements.h"

namespace esphome {
namespace ui_components {

class ImageElement : public BaseElement {
 public:
  void draw(display::Display &disp) override;
  void set_image(ImageProvider *content) { iprovider_ = content; }

 private:
  ImageProvider *iprovider_ = nullptr;
};

}  // namespace ui_components
}  // namespace esphome
