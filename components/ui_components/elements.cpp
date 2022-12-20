#include "elements.h"
#include "esphome/core/log.h"

namespace esphome {

static const char *const TAG = "ui_components";

void ui_components::BaseElement::position_inside(int *content_x, int *content_y, int content_width,
                                                 int content_height) {
  auto x_align = display::TextAlign(int(alignment_) & 0x18);
  auto y_align = display::TextAlign(int(alignment_) & 0x07);
  if (width() > 0) {
    switch (x_align) {
      case display::TextAlign::RIGHT:
        *content_x = x() + width() - content_width;
        break;
      case display::TextAlign::CENTER_HORIZONTAL:
        *content_x = x() + ((width() - content_width) / 2);
        break;
      case display::TextAlign::LEFT:
      default:
        *content_x = x();
        break;
    }
  }

  if (height() > 0) {
    switch (y_align) {
      case display::TextAlign::BOTTOM:
        *content_y = y() + height() - content_height;
        break;
      case display::TextAlign::BASELINE:
        *content_y = y() - *content_y + height() - content_height;
        break;
      case display::TextAlign::CENTER_VERTICAL:
        *content_y = y() + ((height() - content_height) / 2);
        break;
      case display::TextAlign::TOP:
      default:
        *content_y = y();
        break;
    }
  }
}

void ui_components::Primitives::quarter_circle(display::DisplayBuffer &disp, int xCenter, int yCenter, int radius,
                                               Primitives::Corner corner, const Color &color) {
  int f = 1 - radius;
  int ddF_x = 1;
  int ddF_y = -2 * radius;
  int xN = 0;
  int yN = radius;

  while (xN < yN) {
    if (f >= 0) {
      yN--;
      ddF_y += 2;
      f += ddF_y;
    }
    xN++;
    ddF_x += 2;
    f += ddF_x;
    if (corner & Corner::BOTTOM_RIGHT) {
      disp.draw_pixel_at(xCenter + xN, yCenter + yN, color);
      disp.draw_pixel_at(xCenter + yN, yCenter + xN, color);
    }
    if (corner & Corner::TOP_RIGHT) {
      disp.draw_pixel_at(xCenter + xN, yCenter - yN, color); 
      disp.draw_pixel_at(xCenter + yN, yCenter - xN, color);
    }
    if (corner & Corner::BOTTOM_LEFT) {
      disp.draw_pixel_at(xCenter - yN, yCenter + xN, color);  
      disp.draw_pixel_at(xCenter - xN, yCenter + yN, color);
    }
    if (corner & Corner::TOP_LEFT) {
      disp.draw_pixel_at(xCenter - yN, yCenter - xN, color);  
      disp.draw_pixel_at(xCenter - xN, yCenter - yN, color);
    }
  }
}

void ui_components::Primitives::filled_quarter_circle(display::DisplayBuffer &disp, int xCenter, int yCenter,
                                                      int radius, int corner, int offset, const Color &color) {
  int16_t f = 1 - radius;
  int16_t ddF_x = 1;
  int16_t ddF_y = -2 * radius;
  int16_t xN = 0;
  int16_t yN = radius;
  int16_t px = xN;
  int16_t py = yN;

  offset++;  // Avoid some +1's in the loop

  while (xN < yN) {
    if (f >= 0) {
      yN--;
      ddF_y += 2;
      f += ddF_y;
    }
    xN++;
    ddF_x += 2;
    f += ddF_x;
    if (xN < (yN + 1)) {
      if (corner & 1)
        disp.vertical_line(xCenter + xN, yCenter - yN, 2 * yN + offset, color);
      if (corner & 2)
        disp.vertical_line(xCenter - xN, yCenter - yN, 2 * yN + offset, color);
    }
    if (yN != py) {
      if (corner & 1)
        disp.vertical_line(xCenter + py, yCenter - px, 2 * px + offset, color);
      if (corner & 2)
        disp.vertical_line(xCenter - py, yCenter - px, 2 * px + offset, color);
      py = yN;
    }
    px = xN;
  }
}

void ui_components::Primitives::round_rect(display::DisplayBuffer &disp, int x, int y, int width, int height,
                                           int radius, const Color &color) {
  int max_radius = round(((width < height) ? width : height) / 2);  // 1/2 minor axis rounded
  if (radius > max_radius) {
    radius = max_radius;
  }
  disp.horizontal_line(x + radius, y, width - 2 * radius, color);               // Top
  disp.horizontal_line(x + radius, y + height - 1, width - 2 * radius, color);  // Bottom
  disp.vertical_line(x, y + radius, height - 2 * radius, color);                // Left
  disp.vertical_line(x + width - 1, y + radius, height - 2 * radius, color);    // Right
  // draw four corners
  quarter_circle(disp, x + radius, y + radius, radius, Corner::TOP_LEFT, color);
  quarter_circle(disp, x + width - radius - 1, y + radius, radius, Corner::TOP_RIGHT, color);
  quarter_circle(disp, x + width - radius - 1, y + height - radius - 1, radius, Corner::BOTTOM_RIGHT, color);
  quarter_circle(disp, x + radius, y + height - radius - 1, radius, Corner::BOTTOM_LEFT, color);
}

void ui_components::Primitives::filled_round_rect(display::DisplayBuffer &disp, int x, int y, int width, int height,
                                                  int radius, const Color &color) {
  int max_radius = round(((width < height) ? width : height) / 2);  // 1/2 minor axis rounded
  if (radius > max_radius) {
    radius = max_radius;
  }
  disp.filled_rectangle(x + radius, y, width - 2 * radius, height, color);
  // draw four corners
  filled_quarter_circle(disp, x + width - radius - 1, y + radius, radius, 1, height - 2 * radius - 1, color);
  filled_quarter_circle(disp, x + radius, y + radius, radius, 2, height - 2 * radius - 1, color);
}

}  // namespace esphome
