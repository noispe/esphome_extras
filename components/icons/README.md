# Icon Font image provider

A component that allows easier access to "icon fonts" by name instead of the code point.

***External Resources:**

1. TTF files for the icon font.
2. Metadata file for mapping string names to unicode code point.  The tool `buildtables.py` is provided to create it.  It needs python pillow installed and is run as follows:

```shell
$ buildtables.py <input.ttf> <output.json>
```

Icons then can be used by including the `icons` component with the following properties:

- **id**: (Required) The identifier for the icon provider.  Multiples can be used.
- **font_file**: (Required) The filename of the `.ttf` file that contains the icons.
- **metadata_file**: (Required) The `.json` file containing the mapping from the human readable name to the unicode code point.
- **size**: (Required) The size for the icons
- **prefix**: (Optional) A prefix for use with home assistant icons `mdi:icon-name`
- **icons**: A list of icons that should be included in the firmware.

To use the icon with the `esphome::display::DisplayBuffer` call the following function from the display lambda:

```cpp
display::Image *IconProvider::get_icon(const std::string &name)
```

Example:

```yaml
icons:
  - id: icons_small
    font_file: materialdesignicons-webfont.ttf
    metadata_file: materialdesignicons-webfont.json
    size: 24
    prefix: mdi
    icons:
      - gift

display:
  - platform: ...
    lambda: |-
        it.fill(COLOR_ON);
        it.image(0, 0, id(icons_small)->get_icon());
```
