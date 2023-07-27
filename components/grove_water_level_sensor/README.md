# Grove Water Level Sensor

A component to support the [Grove - Water Level Sensor](https://wiki.seeedstudio.com/Grove-Water-Level-Sensor) with esphome.  

## Options

- [Normal sensor component options.](https://esphome.io/components/sensor/index.html)
- **i2c_id**: (Optional) The name of the [i2c bus](https://esphome.io/components/i2c.html) if multiple i2c busses are used. 
- **high_address**: (Optional, default `0x78`) The i2c bus address for the high bytes of the measurement.
- **lower_address**: (Optional, default `0x78`) The i2c bus address for the lower bytes of the measurement.

**NOTE**: The i2c addresses are hardcoded in the sensor itself.

## Example YAML

```yaml
external_components:
  - source:
      type: git
      url: https://gitlab.com/geiseri/esphome_extras.git
      ref: main

sensor:
  - platform: grove_water_level_sensor
    name: Fishtank Water Level
    high_address:  0x78
    low_address:  0x77
    i2c_id: grove

i2c:
  sda: GPIO04
  scl: GPIO13
  id: grove

```