#pragma once

#include "text_element.h"
#include "image_element.h"
#ifdef USE_TOUCHSCREEN
#include "esphome/components/touchscreen/touchscreen.h"
#endif
#ifdef USE_BINARY_SENSOR
#include "esphome/components/binary_sensor/binary_sensor.h"
#endif

namespace esphome {
namespace ui_components {

#if defined(USE_TOUCHSCREEN) && defined(USE_BINARY_SENSOR)
class AbstractButton : public touchscreen::TouchListener {
 public:
  void set_touchscreen(touchscreen::Touchscreen *touchscreen);
  void set_control(binary_sensor::BinarySensor *sensor);
  void touch(touchscreen::TouchPoint tp) override;
  void release() override;
  virtual bool in_range(uint16_t x, uint16_t y) const = 0;

 protected:
  bool is_pressed();
  binary_sensor::BinarySensor *proxy_ = nullptr;
  int radius_ = 10;
};

class TextButtonElement : public TextElement, public AbstractButton {
 public:
  void draw(display::Display &disp) override;
  bool in_range(uint16_t x, uint16_t y) const override;

 protected:
  virtual int margin() const override;
};

class IconButtonElement : public ImageElement, public AbstractButton {
 public:
  void draw(display::Display &disp) override;
  bool in_range(uint16_t x, uint16_t y) const override;

 protected:
  virtual int margin() const override;

 private:
};
#endif

}  // namespace ui_components

}  // namespace esphome
