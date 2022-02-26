#include "box.h"
#include "esphome/core/log.h"

namespace esphome {

static const char *const TAG = "ui_components";

void ui_components::BaseBox::position_inside(int *content_x, int *content_y, int content_width, int content_height) {
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

void textbox::TextBox::draw(display::DisplayBuffer &disp) {
  if (border_) {
    disp.rectangle(x_, y_, width_, height_, fg_color_);
    disp.filled_rectangle(x(), y(), width(), height(), bg_color_);
  }
  const std::string buffer = resolve_string();
  if (buffer.empty()) {
    return;
  }
  int text_x = x();
  int text_y = y();
  int text_width = 0;
  int text_height = 0;
  disp.get_text_bounds(x(), y(), buffer.c_str(), font_, display::TextAlign::TOP_LEFT, &text_y, &text_y, &text_width,
                       &text_height);
  position_inside(&text_x, &text_y, text_width, text_height);

  ESP_LOGD(TAG, "Draw shape at %d,%d - %dx%d", x(), y(), width(), height());
  ESP_LOGD(TAG, "Draw text at %d,%d - %dx%d", text_x, text_y, text_width, text_height);

  disp.print(text_x, text_y, font_, fg_color_, display::TextAlign::TOP_LEFT, buffer.c_str());
}

void imagebox::ImageBox::draw(display::DisplayBuffer &disp) {
  if (border_) {
    disp.rectangle(x_, y_, width_, height_, fg_color_);
  }
  disp.filled_rectangle(x(), y(), width(), height(), bg_color_);
  display::Image *img = content_ == nullptr ? dynamic_content_() : content_;
  if (img == nullptr) {
    return;
  }
  int image_x = x();
  int image_y = y();
  int image_width = img->get_width();
  int image_height = img->get_height();
  position_inside(&image_x, &image_y, image_width, image_height);
  ESP_LOGD(TAG, "Draw shape at %d,%d - %dx%d", x(), y(), width(), height());
  ESP_LOGD(TAG, "Draw image at %d,%d - %dx%d", image_x, image_y, image_width, image_height);
  disp.image(image_x, image_y, img, fg_color_, bg_color_);
}

void template_box::TemplateBox::draw(display::DisplayBuffer &disp) {
  if (border_) {
    disp.rectangle(x_, y_, width_, height_, fg_color_);
  }
  disp.filled_rectangle(x(), y(), width(), height(), bg_color_);
  drawer_(disp, x(), y(), width(), height(), alignment_, fg_color_, bg_color_);
}

std::string textbox::TextBox::resolve_string() {
  switch (type_) {
    case ContentType::SENSOR:
      if (content_->has_state()) {
        return content_->state;
      }
      break;
    case ContentType::DYNAMIC:
      return dynamic_content_();
      break;
    default:
      return static_content_;
      break;
  }
  return static_content_;
}

void ui_components::UIComponents::draw(display::DisplayBuffer &disp) const {
  for (auto &c : content_) {
    c->draw(disp);
  }
}

void ui_components::UIComponents::add_component(BaseBox *box) { content_.push_back(box); }

void shapebox::ShapeBox::draw(display::DisplayBuffer &disp) {
  if (border_) {
    disp.rectangle(x_, y_, width_, height_, fg_color_);
  }
  disp.filled_rectangle(x(), y(), width(), height(), bg_color_);

  switch (shapetype_) {
    case ShapeType::CIRCLE:
      disp.circle(x() + (width() / 2), y() + (height() / 2), std::min(height(), width()), fg_color_);
      break;
    case ShapeType::FILLED_CIRCLE:
      disp.filled_circle(x() + (width() / 2), y() + (height() / 2), std::min(height(), width()), fg_color_);
      break;
    case ShapeType::RECTANGLE:
      disp.rectangle(x(), y(), width(), height(), fg_color_);
      break;
    case ShapeType::FILLED_RECTANGLE:
      disp.filled_rectangle(x(), y(), width(), height(), fg_color_);
      break;
    case ShapeType::LINE:
      if (height_ == 0) {
        disp.horizontal_line(x(), y(), width(), fg_color_);
      } else if (width_ == 0) {
        disp.vertical_line(x(), y(), height(), fg_color_);
      } else {
        disp.line(x(), y(), x() + width(), y() + height(), fg_color_);
      }
      break;
    default:
      break;
  }
}

}  // namespace esphome
