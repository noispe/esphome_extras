#pragma once

#include "Frame.h"
namespace esphome {
namespace controls {

template<typename T>
struct Formatter {
std::string operator()(T*in);
};

template<typename S, typename A> class BasicLabel : public Frame {
 public:
  using update_callback = std::function<void(A)>;
  using format_callback = std::function<std::string(S *)>;
  using render_callback = std::function<void(display::DisplayBuffer, std::string, rect)>;


  BasicLabel();

 protected:
  S *source_ = nullptr;
};
}  // namespace controls
}  // namespace esphome
