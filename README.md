# esphome_extras

Custom components for my various devices.

# Icon Provider

A component that allows easier access to "icon fonts" by name instead of the code point.

[full README](components/icons/README.md)

```yaml
icons:
  - font_file: "cool_icons.ttf"
    metadata_file: "cool_icons.json"
    size: 128
    icons:
      - cool
    id: icon_1

```
# UI Components

A wrapper around creating UI elements that interact with sensors or other resources.  This is the core rendering component for a display or page.

Example:

```yaml
ui_components:
  id: gui_1
 ...
display:
  - platform: ...
    lambda: |-
      id(gui_1)->draw(it);
```

## Boxes

Boxes are a collection of controls that will display specific content on a ui_component.  They all include the following properties:

- **id**: (Required) The unique `ID` of the box
- **renderer**: (Required) The id of the ui_component that will draw the control in the display lambda.
- **x**: (Required) The `x` coordinate relative to the display's `0,0` position.
- **y**: (Required) The `y` coordinate relative to the display's `0,0` position.
- **width**: (Optional) The `width` of the bounding box that contains the box.  If not provided then the contents of the box will be used.
- **height**: (Optional) The `height` of the bounding box that contains the box.  If not provided then the contents of the box will be used.
- **height**: (Optional) The `y` coordinate relative to the display's `0,0` position.
- **alignment**: (Optional) The alignment of the content's bounding rectangle inside of the boxes bounding rectangle.  These match the options of alignment are: `TOP`, `CENTER_VERTICAL`, `BASELINE`, `BOTTOM`, `LEFT`, `CENTER_HORIZONTAL`, `RIGHT`, `TOP_LEFT`, `TOP_CENTER`, `TOP_RIGHT`, `CENTER_LEFT`, `CENTER`, `CENTER_RIGHT`, `BASELINE_LEFT `, `BASELINE_CENTER`, `BASELINE_RIGHT`, `BOTTOM_LEFT`, `BOTTOM_CENTER` and, `BOTTOM_RIGHT`.  It defaults to `TOP_LEFT`.
- **color**: (Optional) The forground color of box's contents.  Defaults to `COLOR_ON`.
- **background**: (Optional) The background color of the box's contents.  Defaults to `COLOR_OFF`
- **border**: (Optional) Draw a thin border around the box using the forground color.  Defaults to `false`.

## TextBox

A box that will display content from a text sensor, lambda or static text.  Either `text` or `text_sensor` is required.

- **text**: (Required) Static text or a template lambda to provide the text to render.  The lambda signature is
```c++
[]() -> std::string{};
```
- **text_sensor**: (Required) The `id` of a `text_sensor` component that will provide the text to render.
- **font**: (Required) The `id` of the font component used to render the text.

```yaml
font:
  - file: "scribble.ttf"
    id: text_normal

text_sensor:
  - platform: version
    name: "ESPHome Version"
    id: version_string

textbox:
  - id: textbox_1
    x: 3
    y: 12
    text: !lambda |
      return "this is a test";
    renderer: gui_1
    font: text_normal
  - id: textbox_2
    x: 3
    y: 12
    text: this is a test
    renderer: gui_1
    font: text_normal
  - id: textbox_3
    x: 3
    y: 12
    text_sensor: version_string
    renderer: gui_1
    font: text_normal
```

## ImageBox

A box that will display an image that can be provided by an `id` or a template lambda.

- **image**: (Required) The `id` of a registered image, or a template lambda that will provide the image. The lambda signature is:
```c++
[]() -> display::Image*{};
```
```yaml
image:
  - file: 3rdparty/sample.jpg
    id: image_1

imagebox:
  - id: imagebox_1
    image: image_1
    x: 3
    y: 12
    renderer: gui_1
  - id: imagebox_2
    image: !lambda |
      return id(image_1);
    x: 3
    y: 12
    renderer: gui_1
```

## ShapeBox

A box that can draw a simple shape.

- **type**: (Required) The type of image to draw inside of the box's defined position and size.  The supported types are: `CIRCLE`,`FILLED_CIRCLE`,`RECTANGLE`,`FILLED_RECTANGLE`, or `LINE`.

```yaml
shapebox:
  - id: line_1
    x: 3
    y: 12
    width: 10
    renderer: gui_1
    type: LINE
  - id: circle_1
    x: 3
    y: 12
    width: 10
    height: 15
    renderer: gui_1
    type: FILLED_CIRCLE
```

## TemplateBox

A box that can render anything supported by `display::DisplayBuffer`

- **lambda**: (Required) The code that will be called for the drawing.  The border, alignment and background are rendered, but the remainder is performed by the lambda.  The lambda signature is:
```c++
[](display::DisplayBuffer &it, int x, int y, int height, int width, display::TextAlign align, Color fg_color, Color bg_color) -> void {};
```
The provided coordinates and size already take into account any border drawn.

```yaml
template_box:
  - id: template_box_1
    x: 3
    y: 12
    renderer: gui_1
    lambda: it.filled_rectangle(x,y,width,height,bg_color);
```

## Testing Locally

inside code-server

```shell
$ docker run --rm --volumes-from codeserver_code_1  -it esphome/esphome compile  <path>/esphome_extras/gui_components.yaml
```

desktop

```shell
$ docker run --rm -v${PWD}:/esphome_extras  -it esphome/esphome compile  /esphome_extras/gui_components.yaml
```
