> **Warning:** I would not consider this to be something to bet your life on.  I was annoyed by the unhelpful datasheet and after wamdering aroud the internet came up with this.

# Theory of operation:

The MQ-9 sensor is a "burn off" sensor.  So there aretwo periods for measurement.  One where the heater is on and one off.  This is achived with a simple circut.

## Circut:

![schematic.png](schematic.png)

[circutjs circut](https://lushprojects.com/circuitjs/circuitjs.html?ctz=CQAgjCAMB0l3BWcMBMcUHYMGZIA4UA2ATmIxAUgpABZsKBTAWjDACgAnEFFGkJwn1x8BfKmHhxO3XuBpDIfMPKjJ4bAC4hsaOUIQo9qiEzzQUpUnnlhChSNkL9s5y5cIJsYN04lUAJgwAZgCGAK4ANhrS2NbgxIY0eOIJxpJsAOa0yfGGCGCG3oZUkGwASjIigkaiqlQ0VEglUNAI5ZVGtuIqVIlULvTNMG0A7tqKRjrdYmxj2Abg9toLRVCz41X6hrWljnjL29Xz26zFa1zHIEcLzX5SAB7ahCZE2ngv9Ep8AGoAlgB2bEeyiQYAw+zAOUhEC+IAA4gAFACSAHl1pdVrElKlSo9HOJIIZsMQIBIkLC4QA5AAibAARtwEOQmCgIMSnO81o9MMRtPQvEhHLQ5CAALIARSYxHRC1qlxKMsM10KKH2pSAA)
## Calibration:

The datasheet provided unhelpfully provides a low resolution diagram of the response.  To extract some useful data from it use https://apps.automeris.io/wpd to modify [calibration data](calibration.json). If you feel you can do better give it a whirl.

![sensor_spec_2019nov0103.jpg](sensor_spec_2019nov0103.jpg)

This image is unhelpfully in a log10/10g10 scale so it needs to be processed before calculation.  To update the calibration parameters run `calibration.py` to refresh `calibration.h`.  This will give a slope that can be followed to calculate ppm.

Once these values are calculated it can be used to convert to the ppm values:

```
ppm = 10 ^ ((log10(Rs/R0) - b) / m)
```

Where `Rs` is found from the AD converter and `R0` is a base value for "clean air"


## Notes on CO:

| PPM     | Typical                                      |
|---------|----------------------------------------------|
| 0.1     | Outside rural area                           |
| 0.5 - 5 | Typical indoor level, or outside urban areas |
| 5 - 15  | Within feet of a gas stove or furnace        |
| 15 - 20 | Typical concentration of furnace vent        |
| 35      | EPA maximum outdoor level                    |
| 50      | Maximum save level for 8 hours               |
| 150     | Polluted city centers                        |
| 400     | Enclosed space with car                      |
| 1,500   | Close to fire                                |
| 6,400   | Undiluted car exhaust                        |

## Notes on TVOC:

| PPB         | Typical   |
|-------------|-----------|
| 0 - 65      | Excellent |
| 65 - 220    | Good      |
| 220 - 660   | Moderate  |
| 660 - 2200  | Poor      |
| 2200 - 5500 | Unhealthy |


# Usage in ESPHome:

Match `AD` to the MQ-9 A0 and `GPIO` to the control in schematic above.  `r0` can be provided or set to `0` in which case a niave attempt will be made to calibrate it based on 30 seconds of `burn off` readings.

```yaml
external_components:
  - source:
      type: git
      url: https://gitlab.com/geiseri/esphome_extras.git
    refresh: 0s

sensor:
  - platform: mq9_circut
    control_pin: GPIO
    adc_pin: AD
    r0: 0
    tvoc:
      name: "TVOC"
    carbon_monoxide:
      name: "CO"
```
