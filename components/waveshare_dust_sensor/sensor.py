import esphome.codegen as cg
import esphome.config_validation as cv
from esphome import pins
from esphome.components import sensor
from esphome.const import (
    CONF_ID,
    CONF_VOLTAGE,
    ICON_GRAIN,
    UNIT_MICROGRAMS_PER_CUBIC_METER,
    DEVICE_CLASS_AQI,
    STATE_CLASS_MEASUREMENT
)

from esphome.components.adc import sensor as adc

CONF_ADC = "adc"
CONF_ILED_PIN = "iled_pin"

waveshare_dust_sensor_ns = cg.esphome_ns.namespace("waveshare_dust_sensor")
WaveshareDustSensor = waveshare_dust_sensor_ns.class_(
    "WaveshareDustSensor", sensor.Sensor, cg.PollingComponent
)

CONFIG_SCHEMA = (
    sensor.sensor_schema(
        unit_of_measurement=UNIT_MICROGRAMS_PER_CUBIC_METER,
        icon=ICON_GRAIN,
        accuracy_decimals=2,
        device_class=DEVICE_CLASS_AQI,
        state_class=STATE_CLASS_MEASUREMENT
    )
    .extend(
        {
            cv.GenerateID(): cv.declare_id(WaveshareDustSensor),
            cv.Required(CONF_ADC): cv.use_id(adc.ADCSensor),
            cv.Required(CONF_ILED_PIN): pins.gpio_output_pin_schema,
        }
    )
    .extend(cv.polling_component_schema("60s"))
)


async def to_code(config):
    var = cg.new_Pvariable(config[CONF_ID])
    await cg.register_component(var, config)
    await sensor.register_sensor(var, config)
    a = await cg.get_variable(config[CONF_ADC])
    cg.add(var.set_adc(a))
    iled_pin = await cg.gpio_pin_expression(config[CONF_ILED_PIN])
    cg.add(var.set_iled_pin(iled_pin))
