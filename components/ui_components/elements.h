#pragma once

#include "esphome/components/display/display_buffer.h"
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

#include "esphome/core/component.h"
#include "esphome/core/color.h"
#include <string>
#include <functional>
#include <vector>

namespace esphome {
namespace ui_components {
class BaseElement {
 public:
  virtual ~BaseElement() = default;

  virtual void draw(display::DisplayBuffer &disp) = 0;

  void set_fg_color(const Color &fg_color) { fg_color_ = fg_color; }
  void set_bg_color(const Color &bg_color) { bg_color_ = bg_color; }
  void set_x(int x) { x_ = x; }
  void set_y(int y) { y_ = y; }
  void set_width(int width) { width_ = width; }
  void set_height(int height) { height_ = height; }
  void set_alignment(const display::TextAlign &alignment) { alignment_ = alignment; }
  void set_border(bool border) { border_ = border; }

 protected:
  void position_inside(int *content_x, int *content_y, int content_width, int content_height);
  int x() const { return border_ ? x_ + 1 : x_; }
  int y() const { return border_ ? y_ + 1 : y_; };
  int width() const { return border_ ? width_ - 2 : width_; }
  int height() const { return border_ ? height_ - 2 : height_; };
  Color fg_color_ = display::COLOR_ON;
  Color bg_color_ = display::COLOR_OFF;
  int x_ = 0;
  int y_ = 0;
  int width_ = 0;
  int height_ = 0;
  bool border_ = false;
  display::TextAlign alignment_ = display::TextAlign::TOP_LEFT;
};

class UIComponents {
 public:
  void draw(display::DisplayBuffer &disp) const;
  void set_content(const std::vector<BaseElement *> &content) { content_ = content; }

 protected:
  std::vector<BaseElement *> content_{};
};

class TextElement : public BaseElement {
 public:
  void draw(display::DisplayBuffer &disp) override;
  void set_font(display::Font *font) { font_ = font; }
#ifdef USE_SENSOR
  void set_content(sensor::Sensor *content);
#endif
#ifdef USE_BINARY_SENSOR
  void set_content(binary_sensor::BinarySensor *content);
#endif
#ifdef USE_TEXT_SENSOR
  void set_content(text_sensor::TextSensor *content);
#endif
  void set_content(const std::string &content);
  void set_content(const std::function<std::string()> &content) { dynamic_content_ = content; }
  void set_default(const std::string &d) { default_ = d; }

 private:
  display::Font *font_ = nullptr;
  std::function<std::string()> dynamic_content_{};
  std::string default_;
};

class ImageElement : public BaseElement {
 public:
  void draw(display::DisplayBuffer &disp) override;
  void set_image(display::Image *content) {
    set_image([content]() { return content; });
  }
  void set_image(std::function<display::Image *()> dynamic_content) { dynamic_content_ = dynamic_content; }

#if defined(USE_TEXT_SENSOR) && defined(USE_ICON_PROVIDER)
  void set_image(text_sensor::TextSensor *content, icon_provider::IconProvider *icon);
#endif

 private:
  std::function<display::Image *()> dynamic_content_{};
};

class TemplateElement : public BaseElement {
 public:
  using drawing_template_t =
      std::function<void(display::DisplayBuffer &, int, int, int, int, display::TextAlign, Color, Color)>;
  void draw(display::DisplayBuffer &disp) override;
  void set_drawer(drawing_template_t drawer) { drawer_ = drawer; }

 private:
  drawing_template_t drawer_;
};

class ShapeElement : public BaseElement {
 public:
  enum class ShapeType {
    CIRCLE,
    FILLED_CIRCLE,
    RECTANGLE,
    FILLED_RECTANGLE,
    LINE,
  };

  void draw(display::DisplayBuffer &disp) override;
  void set_shape_type(ShapeType shapetype) { shapetype_ = shapetype; }

 private:
  ShapeType shapetype_;
};
}  // namespace ui_components

}  // namespace esphome
