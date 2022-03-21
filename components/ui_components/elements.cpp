#include "elements.h"
#include "esphome/core/log.h"
#include <sstream>
#include <iomanip>

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

void ui_components::TextElement::draw(display::DisplayBuffer &disp) {
  if (!visible_) {
    return;
  }

  if (border_) {
    disp.rectangle(x_, y_, width_, height_, fg_color_);
    disp.filled_rectangle(x(), y(), width(), height(), bg_color_);
  }
  const std::string buffer = dynamic_content_();
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

void ui_components::ImageElement::draw(display::DisplayBuffer &disp) {
  if (border_) {
    disp.rectangle(x_, y_, width_, height_, fg_color_);
  }
  disp.filled_rectangle(x(), y(), width(), height(), bg_color_);
  display::Image *img = dynamic_content_();
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

void ui_components::TemplateElement::draw(display::DisplayBuffer &disp) {
  if (border_) {
    disp.rectangle(x_, y_, width_, height_, fg_color_);
  }
  disp.filled_rectangle(x(), y(), width(), height(), bg_color_);
  drawer_(disp, x(), y(), width(), height(), alignment_, fg_color_, bg_color_);
}

void ui_components::UIComponents::draw(display::DisplayBuffer &disp) const {
  for (auto &c : content_) {
    c->draw(disp);
  }
}

void ui_components::ShapeElement::draw(display::DisplayBuffer &disp) {
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

#ifdef USE_SENSOR
void ui_components::TextElement::set_content(sensor::Sensor *content) {
  set_content([this, content]() {
    if (content->has_state()) {
      std::ostringstream ss;
      ss << std::setw(content->get_accuracy_decimals()) << std::fixed << content->get_state()
         << content->get_unit_of_measurement();
      return ss.str();
    }
    return this->default_;
  });
}
#endif
#ifdef USE_TEXT_SENSOR
void ui_components::TextElement::set_content(text_sensor::TextSensor *content) {
  set_content([this, content]() { return content->has_state() ? content->get_state() : this->default_; });
}
#endif
#ifdef USE_BINARY_SENSOR
void ui_components::TextElement::set_content(binary_sensor::BinarySensor *content) {
  set_content([this, content]() {
    if (content->has_state()) {
      return content->state ? std::string{"ON"} : std::string{"OFF"};
    }
    return this->default_;
  });
}
#endif

void ui_components::TextElement::set_content(const std::string &content) {
  set_content([content]() { return content; });
}
#if defined(USE_TEXT_SENSOR) && defined(USE_ICON_PROVIDER)
void ui_components::ImageElement::set_image(text_sensor::TextSensor *content, icon_provider::IconProvider *icon) {
  set_image([content, icon]() -> display::Image * {
    if (content->has_state()) {
      return icon->get_icon(content->get_state());
    }
    return nullptr;
  });
}
#endif

void ui_components::ButtonElement::set_button(binary_sensor::BinarySensor *button) {
  button_ = button;
  button_->add_on_state_callback([this](bool state) {

  });
}
void ui_components::ButtonElement::set_touchscreen(touchscreen::Touchscreen *touchscreen) {
  touchscreen->register_listener(this);
}
void ui_components::TextButtonElement::set_override_text(const std::string &text) { override_text_ = text; };
void ui_components::ButtonElement::touch(touchscreen::TouchPoint tp) {
  if (!visible_) {
    return;
  }
  if (tp.x < this->x()) {
    return;
  }
  if (tp.x > this->x() + this->width()) {
    return;
  }
  if (tp.y < this->y()) {
    return;
  }
  if (tp.y > this->y() + this->height()) {
    return;
  }
  button_->publish_state(true);
}
void ui_components::ButtonElement::release() { button_->publish_state(false); }
void ui_components::TextButtonElement::draw(display::DisplayBuffer &disp) {
  auto fg = button_->state ? bg_color_ : fg_color_;
  auto bg = button_->state ? fg_color_ : bg_color_;

  if (border_) {
    disp.rectangle(x_, y_, width_, height_, fg_color_);
    disp.filled_rectangle(x(), y(), width(), height(), bg_color_);
  }

  disp.filled_rectangle(x(), y(), width(), height(), bg);
  std::string buffer = override_text_.empty() ? button_->get_name() : override_text_;
  int text_x = x();
  int text_y = y();
  int text_width = 0;
  int text_height = 0;
  disp.get_text_bounds(x(), y(), buffer.c_str(), font_, display::TextAlign::TOP_LEFT, &text_y, &text_y, &text_width,
                       &text_height);
  position_inside(&text_x, &text_y, text_width, text_height);
  disp.print(text_x, text_y, font_, fg, display::TextAlign::TOP_LEFT, buffer.c_str());
}

#if defined(USE_TEXT_SENSOR) && defined(USE_ICON_PROVIDER)
void ui_components::IconButtonElement::set_image(text_sensor::TextSensor *content, icon_provider::IconProvider *icon) {
  set_image([content, icon]() -> display::Image * {
    if (content->has_state()) {
      return icon->get_icon(content->get_state());
    }
    return nullptr;
  });
}
#endif

void ui_components::IconButtonElement::draw(display::DisplayBuffer &disp) {
  auto fg = button_->state ? bg_color_ : fg_color_;
  auto bg = button_->state ? fg_color_ : bg_color_;

  if (border_) {
    disp.rectangle(x_, y_, width_, height_, fg_color_);
    disp.filled_rectangle(x(), y(), width(), height(), bg_color_);
  }

  disp.filled_rectangle(x(), y(), width(), height(), bg_color_);
  display::Image *img = dynamic_content_();
  if (img == nullptr) {
    return;
  }
  int image_x = x();
  int image_y = y();
  int image_width = img->get_width();
  int image_height = img->get_height();
  position_inside(&image_x, &image_y, image_width, image_height);
  disp.image(image_x, image_y, img, fg_color_, bg_color_);
}
}  // namespace esphome
