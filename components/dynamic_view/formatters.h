#pragma once

#include <fmt/format.h>

template <> struct fmt::formatter<esphome::sensor::Sensor*>: fmt::formatter<fmt::string_view> {
  // parse is inherited from formatter<string_view>.
  template <typename FormatContext>
  auto format(esphome::sensor::Sensor *s, FormatContext& ctx) -> decltype(ctx.out()) {
    string_view name = "unknown";
    switch (c) {
    case color::red:   name = "red"; break;
    case color::green: name = "green"; break;
    case color::blue:  name = "blue"; break;
    }
    return formatter<string_view>::format(name, ctx);
  }
};

template <> struct fmt::formatter<esphome::sensor::Sensor*> {
  // Presentation format: 'f' - fixed, 'e' - exponential.
  std::string internal_{};

  // Parses format specifications of the form ['f' | 'e'].
  constexpr auto parse(format_parse_context& ctx) -> decltype(ctx.begin()) {
    // [ctx.begin(), ctx.end()) is a character range that contains a part of
    // the format string starting from the format specifications to be parsed,
    // e.g. in
    //
    //   fmt::format("{:f} - point of interest", point{1, 2});
    //
    // the range will contain "f} - point of interest". The formatter should
    // parse specifiers until '}' or the end of the range. In this example
    // the formatter should parse the 'f' specifier and return an iterator
    // pointing to '}'.

    // Parse the presentation format and store it in the formatter:
    auto it = ctx.begin(), end = ctx.end();
    if (it != end && (*it == 'f' || *it == 'e')) presentation = *it++;

    // Check if reached the end of the range:
    if (it != end && *it != '}')
      throw format_error("invalid format");

    // Return an iterator past the end of the parsed range:
    return it;
  }

  // Formats the point p using the parsed format specification (presentation)
  // stored in this formatter.
  template <typename FormatContext>
  auto format(const point& p, FormatContext& ctx) -> decltype(ctx.out()) {
    // ctx.out() is an output iterator to write to.
    return format_to(
        ctx.out(),
        presentation == 'f' ? "({:.1f}, {:.1f})" : "({:.1e}, {:.1e})",
        p.x, p.y);
  }
};
