# esphome_extras

Custom components for my various devices.

# Icon Provider

A component that allows easier access to "icon fonts" by name instead of the code point.

[Full README here](components/icons/README.md)

Example:

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

[Full README here](components/ui_components/README.md)

## Testing Locally

inside code-server

```shell
$ docker run --rm --volumes-from codeserver_code_1  -it esphome/esphome compile  <path>/esphome_extras/gui_components.yaml
```

desktop

```shell
$ docker run --rm -v${PWD}:/esphome_extras  -it esphome/esphome compile  /esphome_extras/gui_components.yaml
```
