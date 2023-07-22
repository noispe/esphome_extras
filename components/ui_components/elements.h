#pragma once

#include "esphome/components/display/display.h"
#include "esphome/components/font/font.h"
#include "esphome/core/component.h"
#include "esphome/core/color.h"
#include "esphome/core/helpers.h"
#include <string>
#include <functional>
#include <tuple>

namespace esphome {
namespace ui_components {

class BaseUiComponent {
 public:
  BaseUiComponent() = default;
  virtual ~BaseUiComponent() = default;

  virtual void draw(display::Display &disp) = 0;

  Color default_fg() const { return default_fg_; }
  void set_default_fg(const Color &default_fg) { default_fg_ = default_fg; }

  Color default_bg() const { return default_bg_; }
  void set_default_bg(const Color &default_bg) { default_bg_ = default_bg; }

  font::Font *default_font() const { return default_font_; }
  void set_default_font(font::Font *default_font) { default_font_ = default_font; }

 private:
  Color default_fg_ = Color::BLACK;
  Color default_bg_ = Color::WHITE;
  font::Font *default_font_ = nullptr;
};

template<typename... Args> class UIComponents : public BaseUiComponent {
 public:
  UIComponents(Args... args) : content_(std::make_tuple(args...)) {}
  void draw(display::Display &disp) override {
    draw_helper(disp, this->content_, typename gens<sizeof...(Args)>::type());
  }

 protected:
  template<int... Is>
  void draw_helper(display::Display &disp, const std::tuple<Args...> &t, seq<Is...> /*meta*/) {
    static_cast<void>(std::initializer_list<char>{(static_cast<void>(std::get<Is>(t)->draw(disp)), '0')...});
  }

  std::tuple<Args...> content_{};
};

class BaseElement : public Parented<BaseUiComponent> {
 public:
  virtual ~BaseElement() = default;

  virtual void draw(display::Display &disp) = 0;

  void set_fg_color(const Color &fg_color) { fg_color_ = fg_color; }
  void set_bg_color(const Color &bg_color) { bg_color_ = bg_color; }
  void set_x(int x) { x_ = x; }
  void set_y(int y) { y_ = y; }
  void set_width(int width) { width_ = width; }
  void set_height(int height) { height_ = height; }
  void set_alignment(const display::TextAlign &alignment) { alignment_ = alignment; }
  void set_border(bool border) { border_ = border; }
  void set_visible(bool visible) { visible_ = visible; }
  bool get_visible() { return visible_; }
  Color get_fg_color() const { return fg_color_.has_value() ? fg_color_.value() : parent_->default_fg();}
  Color get_bg_color() const  { return bg_color_.has_value() ? bg_color_.value() : parent_->default_bg();};
 protected:
  void position_inside(int *content_x, int *content_y, int content_width, int content_height);
  int x() const { return margin() + border_ ? x_ + 1 : x_; }
  int y() const { return margin() + border_ ? y_ + 1 : y_; };
  int width() const { return (-2 * margin()) + border_ ? width_ - 2 : width_; }
  int height() const { return (-2 * margin()) + border_ ? height_ - 2 : height_; };
  virtual int margin() const { return 0; }
  optional<Color> fg_color_;
  optional<Color> bg_color_;
  int x_ = 0;
  int y_ = 0;
  int width_ = 0;
  int height_ = 0;
  bool border_ = false;
  display::TextAlign alignment_ = display::TextAlign::TOP_LEFT;
  bool visible_ = true;
};

// Move to displaybuffer at some point
struct Primitives {
  enum Corner { TOP_RIGHT = 0x4, TOP_LEFT = 0x2, BOTTOM_LEFT = 0x1, BOTTOM_RIGHT = 0x8 };

  static void quarter_circle(display::Display &disp, int xCenter, int yCenter, int radius,
                             Primitives::Corner corner, const Color &color);
  static void filled_quarter_circle(display::Display &disp, int xCenter, int yCenter, int radius, int corner,
                                    int offset, const Color &color);
  static void round_rect(display::Display &disp, int x, int y, int width, int height, int radius,
                         const Color &color);
  static void filled_round_rect(display::Display &disp, int x, int y, int width, int height, int radius,
                                const Color &color);
};

}  // namespace ui_components

}  // namespace esphome
