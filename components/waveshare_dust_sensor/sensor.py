import esphome.codegen as cg
import esphome.config_validation as cv
from esphome import pins
from esphome.components import sensor, voltage_sampler
from esphome.const import (
    CONF_ID,
    ICON_GRAIN,
    UNIT_MICROGRAMS_PER_CUBIC_METER,
)

AUTO_LOAD = ["voltage_sampler"]

CONF_ADC_PIN = "adc_pin"
CONF_ILED_PIN = "iled_pin"

waveshare_dust_sensor_ns = cg.esphome_ns.namespace("waveshare_dust_sensor")
WaveshareDustSensor = waveshare_dust_sensor_ns.class_(
    "WaveshareDustSensor", sensor.Sensor, cg.PollingComponent, voltage_sampler.VoltageSampler
)

CONFIG_SCHEMA = (
    sensor.sensor_schema(
        UNIT_MICROGRAMS_PER_CUBIC_METER, ICON_GRAIN, 2
    )
    .extend(
        {
            cv.GenerateID(): cv.declare_id(WaveshareDustSensor),
            cv.Required(CONF_ADC_PIN): pins.analog_pin,
            cv.Required(CONF_ILED_PIN): pins.gpio_output_pin_schema,
        }
    )
    .extend(cv.polling_component_schema("60s"))
)


def to_code(config):
    var = cg.new_Pvariable(config[CONF_ID])
    yield cg.register_component(var, config)
    yield sensor.register_sensor(var, config)
    cg.add(var.set_adc_pin(config[CONF_ADC_PIN]))
    iled_pin = yield cg.gpio_pin_expression(config[CONF_ILED_PIN])
    cg.add(var.set_iled_pin(iled_pin))