# UI Components

A wrapper around creating UI elements that interact with sensors or other resources.  This is the core rendering component for a display or page.

Example:

## Component

The toplevel `ui_components` is used to group the elements that it will render in the `display`'s `lambda` property.  To invoke the rendering of the elements in the lambda call:

```cpp
void UIComponents::draw(display::Display &it)
```

The following properties configure the component:

- **id**: (Required) The unique identifier used to resolve the `draw` method for the display.
- **elements**: (Required) A list of one or more Elements.

```yaml
ui_components:
  id: gui_1
  elements: ...
 ...
display:
  - platform: ...
    lambda: |-
      id(gui_1)->draw(it);
```

## Elements

Elements are objects that will display specific content on a ui_component.  They all include the following properties:

- **x**: (Required) The `x` coordinate relative to the display's `0,0` position.
- **y**: (Required) The `y` coordinate relative to the display's `0,0` position.
- **width**: (Optional) The `width` of the bounding box that contains the element.  If not provided then the contents of the element will be used.
- **height**: (Optional) The `height` of the bounding box that contains the element.  If not provided then the contents of the element will be used.
- **height**: (Optional) The `y` coordinate relative to the display's `0,0` position.
- **alignment**: (Optional) The alignment of the content's bounding rectangle inside of the element's bounding rectangle.  These match the options of alignment are: `TOP`, `CENTER_VERTICAL`, `BASELINE`, `BOTTOM`, `LEFT`, `CENTER_HORIZONTAL`, `RIGHT`, `TOP_LEFT`, `TOP_CENTER`, `TOP_RIGHT`, `CENTER_LEFT`, `CENTER`, `CENTER_RIGHT`, `BASELINE_LEFT `, `BASELINE_CENTER`, `BASELINE_RIGHT`, `BOTTOM_LEFT`, `BOTTOM_CENTER` and, `BOTTOM_RIGHT`.  It defaults to `TOP_LEFT`.
- **color**: (Optional) The foreground color of the element's contents.  Defaults to `COLOR_ON`.
- **background**: (Optional) The background color of the element's contents.  Defaults to `COLOR_OFF`
- **border**: (Optional) Draw a thin border around the element using the foreground color.  Defaults to `false`.

## TextElement

An element that will display content from a text sensor, lambda or static text.  One of either `text`, `text_sensor`,`binary_sensor`, or `sensor` is required.

- **text**: (Required) Static text or a template lambda to provide the text to render.  The lambda signature is
```c++
[]() -> std::string{};
```
- **text_sensor**: (Required) The `id` of a `text_sensor` component that will provide the text to render.
- **binary_sensor**: (Required) The `id` of a `binary_sensor` component that will provide the text to render.  The values will be displayed as `ON` or `OFF`.
- **sensor**: (Required) The `id` of a `sensor` component that will be formatted to provide the text to render.
- **font**: (Required) The `id` of the font component used to render the text.
- **default**: (Optional) The default text to display when the associated sensor has no state.  If not populated an empty string is used.

```yaml
font:
  - file: "scribble.ttf"
    id: text_normal

binary_sensor:
  - platform: homeassistant
    id: remote_entity

text_sensor:
  - platform: version
    name: "ESPHome Version"
    id: version_string

ui_components:
  id: gui_1
  elements:
    - text:
        x: 3
        y: 12
        text: !lambda |
          return "this is a test";
        font: text_normal
    - text:
        x: 3
        y: 12
        text: this is a test;
        font: text_normal
    - text:
        x: 3
        y: 12
        text_sensor: version_string
        default: "No data"
        font: text_normal
    - text:
        x: 3
        y: 12
        sensor: remote_entity
        font: text_normal
```

## ImageElement

An element that will display an image that can be provided by an `id` or a template lambda.

- **image**: (Required) The `id` of a registered image, or a template lambda that will provide the image. The lambda signature is:
```c++
[]() -> image::Image*{};
```
```yaml
image:
  - file: 3rdparty/sample.jpg
    id: image_1
ui_components:
  id: gui_1
  elements:
    - image:
        image: image_1
        x: 3
        y: 12
    - image:
        image: !lambda |
          return id(image_1);
        x: 3
        y: 12
```

## ShapeElement

An element that can draw a simple shape.

- **shape**: (Required) The type of shape to draw inside of the elements's defined position and size.  The supported types are: `CIRCLE`,`FILLED_CIRCLE`,`RECTANGLE`,`FILLED_RECTANGLE`, or `LINE`.

```yaml
ui_components:
  id: gui_1
  elements:
    - shape:
        x: 3
        y: 12
        width: 10
        shape: LINE
    - shape:
        x: 3
        y: 12
        width: 10
        height: 15
        shape: FILLED_CIRCLE
```

## TemplateElement

An element that can render anything supported by `display::Display`

- **lambda**: (Required) The code that will be called for the drawing.  The border, alignment and background are rendered, but the remainder is performed by the lambda.  The lambda signature is:
```c++
[](display::Display &it, int x, int y, int height, int width, display::TextAlign align, Color fg_color, Color bg_color) -> void {};
```
The provided coordinates and size already take into account any border drawn.

```yaml
ui_components:
  id: gui_1
  elements:
    - template:
        x: 3
        y: 12
        lambda: it.filled_rectangle(x,y,width,height,bg_color);
```
