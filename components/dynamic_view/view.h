#pragma once
#include "esphome/components/display/display_buffer.h"
#include "esphome/core/component.h"
#include "esphome/core/defines.h"

#include <fmt/format.h>

#include <map>
#include <vector>

/*
template<typename T>
void draw_XXX_element(DisplayBufferRef disp, int x, int y, int width, int
height, uint8_t color, T *ent);

  void line(int x1, int y1, int x2, int y2, Color color = COLOR_ON);
  void rectangle(int x1, int y1, int width, int height, Color color = COLOR_ON);
  void filled_rectangle(int x1, int y1, int width, int height, Color color =
COLOR_ON); void printf(int x, int y, Font *font, Color color, TextAlign align,
const char *format, ...) void strftime(int x, int y, Font *font, Color color,
TextAlign align, const char *format, time::ESPTime time) void image(int x, int
y, Image *image, Color color_on = COLOR_ON, Color color_off = COLOR_OFF);

util:
 void print(int x, int y, Font *font, Color color, TextAlign align, const char
*text); void get_text_bounds(int x, int y, const char *text, Font *font,
TextAlign align, int *x1, int *y1, int *width, int *height);

api:

*/
namespace esphome {

namespace dynamic_view {
class Cursor {
  DisplayBufferRef display_;
  int left_;
  int right_;
  int top_;
  int bottom_;
  int x_ = 0;
  int y_ = 0;
  Font *font_ = nullptr;
  uint8_t fg_color_ = 0xFF;
  uint8_t bg_color_ = 0x00;

  std::string format_(fmt::string_view format, fmt::format_args args) {
    return fmt::vformat(format, args);
  }

  int print_(bool full_line, esphome::display::TextAlign alignment,
             const S &format, Args &&... args) {
    std::string formated =
        format_(format, fmt::make_args_checked<Args...>(format, args...));
    int x, y, width, height = 0;
    display_->get_text_bounds(0, 0, formatted.c_str(), font_, alignment, &x, &y,
                              &width, &height);
    display_->filled_rectangle(x + x_, y + y_, width, height, bg_color_);
    display_->print(x_, y_, font_, fg_color_, alignment, formated.c_str());
    return std::make_pair(x + width, y + height);
    return full_line ? y + height : x + width;
  }

public:
  Cursor(DisplayBufferRef display, int left, int right, int top, int bottom);
  Cursor subcursor(int height, int width);
  void set_font(Font *f);
  void set_color(uint8t_ c);
  void set_background_color(uint8t_ c);
  void set_x(int x);
  void set_y(int y);
  void up(int delta = 1);
  void down(int delta = 1);
  void left(int delta = 1);
  void right(int delta = 1);
  void draw_line(int xdelta, int ydelta);
  void draw_rect(int xdelta, int ydelta, bool fill = false);
  void draw_hline(int y);
  void draw_vline(int x);
  void draw_hline();
  void draw_vline();
  void left_edge();
  void right_edge();
  void top_edge();
  void bottom_edge();
  template <typename S, typename... Args>
  void print(esphome::display::TextAlign alignment, const S &format,
             Args &&... args) {
    x_ += print_(false, alignment, format, args...);
  }

  template <typename S, typename... Args>
  void print(const S &format, Args &&... args) {
    print(esphome::display::TextAlign::TOP_LEFT, format, args...);
  }

  template <typename S, typename... Args>
  void println(esphome::display::TextAlign alignment, const S &format,
               Args &&... args) {
    y_ += print_(true, alignment, format, args...);
  }

  template <typename S, typename... Args>
  void println(const S &format, Args &&... args) {
    println(esphome::display::TextAlign::TOP_LEFT, format, args...);
  }
};

class View : public esphome::Controller {
  std::function<void(DisplayBufferRef)> render_;

public:
  void set_render_hook(std::function<void(DisplayBufferRef)> hook) {
    render_ = hook;
  }
  void render(DisplayBufferRef *display) const { render_(display); }
};
} // namespace dynamic_view
} // namespace esphome
