# Icon Font image provider

A shortcut to include icon fonts into your esphome UI.

External Resources:

1. ttf files for the icon font.
2. metadata for mapping string names to unicode code point.  The tool `buildtables.py` is provided to create it.


Icons then can be used by including the component.

- id: identifier for the icon provider.  Multiples can be used.
- font_file: the filename of the `.ttf` file that contains the icons.
- metadata_file: the `.json` file containing the mapping from the human readable name to the unicode code point.
- size: the size for the icons
- prefix: an optional prefix for use with home assistant icons `mdi:icon-name`
- icons: a list of icons that should be included in the firmware.

```yaml
icons:
  - id: icons_small
    font_file: materialdesignicons-webfont.ttf
    metadata_file: materialdesignicons-webfont.json
    size: 24
    prefix: mdi
    icons:
      - calendar-blank
      - gift
      - gift-open
      - calendar-star
      - cellphone
      - cellphone-off
      - television
      - television-off
```

To get the font for use with the `esphome::display::DisplayBuffer`:

```cpp
id(icon_id)->get_font()
```
To get the charicter used to display the font use:
```cpp
id(icon_id)->get_icon("iconname")
```

```yaml
pages:
  - id: info_page
    lambda: |-
        it.fill(COLOR_ON);
        it.print(10,20,id(icons_small)->get_font(), COLOR_OFF,  id(icons_small)->get_icon(id(tv_time_icon).state));
        it.printf(75,10,id(text_normal), COLOR_OFF, "%d minutes of TV time", (int)id(tv_time).state);
        it.print(10,85 + 5,id(icons_small)->get_font(), COLOR_OFF,  id(icons_small)->get_icon(id(tablet_time_icon).state));
        it.printf(75,85,id(text_normal), COLOR_OFF, "%d minutes of tablet time", (int)id(tablet_time).state);
```
