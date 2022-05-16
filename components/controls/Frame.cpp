#include "Frame.h"

namespace esphome
{
namespace controls{

void Frame::set_display(display::DisplayBuffer *display) {
  display_ = display;
}}

void controls::Frame::set_geometry(const Geometry &g) {
  geometry = g;
}

} // namespace esphome
