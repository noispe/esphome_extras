#include "view.h"

esphome::dynamic_view::Cursor::Cursor(DisplayBufferRef display, int left,
                                      int right, int top, int bottom)
    : display_(display), left_(left), right_(right), top_(top), bottom_(bottom),
      x_(left), y_(top) {}

void esphome::dynamic_view::Cursor::set_font(Font *f) { font_ = f; }

void esphome::dynamic_view::Cursor::set_color(uint8t_ c) { fg_color_ = c; }

void esphome::dynamic_view::Cursor::set_background_color(uint8t_ c) {
  bg_color_ = c;
}

void esphome::dynamic_view::Cursor::up(int delta) { y_ -= delta; }

void esphome::dynamic_view::Cursor::down(int delta) { y_ += delta; }

void esphome::dynamic_view::Cursor::left(int delta) { x_ -= delta; }

void esphome::dynamic_view::Cursor::right(int delta) { x_ += delta; }

void esphome::dynamic_view::Cursor::draw_line(int xdelta, int ydelta) {
  int new_x = x + xdelda;
  int new_y = y + ydelda;
  display_->line(x_, y_, new_x, new_y, fg_color_);
  x_ = new_x;
  y_ = new_y;
}

void esphome::dynamic_view::Cursor::draw_rect(int xdelta, int ydelta,
                                              bool fill) {
  int new_x = x + xdelda;
  int new_y = y + ydelda;
  if (fill) {
    display_->filled_rectangle(x_, y_, xdelta, ydelta, fg_color_);
  } else {
    display_->rectangle(x_, y_, xdelta, ydelta, fg_color_);
  }
  x_ = new_x;
  y_ = new_y;
}

void esphome::dynamic_view::Cursor::draw_hline(int y) {
  display_->line(left_, y, right_, y, fg_color_);
  y_ = y + 1;
}

void esphome::dynamic_view::Cursor::draw_vline(int x) {
  display_->line(x, top_, x, bottom_, fg_color_);
  x_ = x + 1;
}

void esphome::dynamic_view::Cursor::left_edge() { x_ = left_; }

void esphome::dynamic_view::Cursor::right_edge() { x_ = right_; }

void esphome::dynamic_view::Cursor::top_edge() { y_ = top_; }

void esphome::dynamic_view::Cursor::bottom_edge() { y_ = bottom_; }

Cursor esphome::dynamic_view::Cursor::subcursor(int height, int width) {
  // DisplayBufferRef display, int left, int right, int top, int bottom
  Cursor cur{display_, x_, x_ + width, y_, y_ + height};
  cur.set_font(font_);
  cur.set_color(fg_color_);
  cur.set_background_color(bg_color_);
  return std::move(cur);
}

void esphome::dynamic_view::Cursor::set_y(int y) { y_ = y; }

void esphome::dynamic_view::Cursor::set_x(int x) { x_ = x; }

void esphome::dynamic_view::Cursor::draw_vline() {
  draw_vline(x_);
}

void esphome::dynamic_view::Cursor::draw_hline() {
  draw_hline(y_);
}
