import esphome.codegen as cg
import esphome.config_validation as cv
from esphome import pins
from esphome.components import sensor, voltage_sampler
from esphome.const import (
    CONF_ID,
    UNIT_PARTS_PER_MILLION,
    UNIT_PARTS_PER_BILLION,
    CONF_TVOC,
    DEVICE_CLASS_EMPTY,
    DEVICE_CLASS_CARBON_MONOXIDE,
    DEVICE_CLASS_GAS,
    ICON_CHEMICAL_WEAPON,
    STATE_CLASS_MEASUREMENT
)

AUTO_LOAD = ["voltage_sampler"]

CONF_ADC_PIN = "adc_pin"
CONF_CONTROL_PIN = "control_pin"
CONF_CO = "carbon_monoxide"
CONF_R0 = "r0"
CONF_R0_CALIBRATE = "r0_calibrate"
mq9_circut_ns = cg.esphome_ns.namespace("mq9_circut")
Mq9Circut = mq9_circut_ns.class_(
    "Mq9Circut", sensor.Sensor, cg.Component, voltage_sampler.VoltageSampler
)

CONFIG_SCHEMA = cv.All(
    cv.Schema(
        {
            cv.GenerateID(): cv.declare_id(Mq9Circut),
            cv.Required(CONF_ADC_PIN): pins.analog_pin,
            cv.Required(CONF_CONTROL_PIN): pins.gpio_output_pin_schema,
            cv.Optional(CONF_R0): cv.float_range(
                min=0.001, max=1000
            ),
            cv.Optional(CONF_R0_CALIBRATE): sensor.sensor_schema(
                accuracy_decimals=3,
                device_class=DEVICE_CLASS_EMPTY,
                state_class=STATE_CLASS_MEASUREMENT,
            ),
            cv.Optional(CONF_TVOC): sensor.sensor_schema(
                UNIT_PARTS_PER_BILLION,
                ICON_CHEMICAL_WEAPON,
                2,
                DEVICE_CLASS_GAS,
                STATE_CLASS_MEASUREMENT,
            ),
            cv.Optional(CONF_CO): sensor.sensor_schema(
                UNIT_PARTS_PER_MILLION,
                ICON_CHEMICAL_WEAPON,
                3,
                DEVICE_CLASS_CARBON_MONOXIDE,
                STATE_CLASS_MEASUREMENT,
            )
        }).extend(cv.COMPONENT_SCHEMA),
    cv.has_exactly_one_key(CONF_R0_CALIBRATE, CONF_R0),
)


async def to_code(config):
    var = cg.new_Pvariable(config[CONF_ID])
    await cg.register_component(var, config)
    cg.add(var.set_adc_pin(config[CONF_ADC_PIN]))
    control_pin = await cg.gpio_pin_expression(config[CONF_CONTROL_PIN])
    cg.add(var.set_control_pin(control_pin))
    if CONF_R0 in config:
        cg.add(var.set_r0(config[CONF_R0]))
    if CONF_R0_CALIBRATE in config:
        sens = await sensor.new_sensor(config[CONF_R0_CALIBRATE])
        cg.add(var.set_calibration_sensor(sens))
    if CONF_TVOC in config:
        sens = await sensor.new_sensor(config[CONF_TVOC])
        cg.add(var.set_tvoc_sensor(sens))
    if CONF_CO in config:
        sens = await sensor.new_sensor(config[CONF_CO])
        cg.add(var.set_co_sensor(sens))
