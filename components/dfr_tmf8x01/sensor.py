import esphome.codegen as cg
import esphome.config_validation as cv
from esphome import pins
from esphome.components import i2c, sensor
from esphome.const import (
    CONF_ID,
    CONF_ENABLE_PIN,
    ICON_ARROW_EXPAND_VERTICAL,
    DEVICE_CLASS_EMPTY,
    STATE_CLASS_MEASUREMENT,
    UNIT_METER
)

DEPENDENCIES = ["i2c"]

dfr_tmf8x01_ns = cg.esphome_ns.namespace("dfr_tmf8x01")
DfrTmf8x01Sensor = dfr_tmf8x01_ns.class_(
    "DfrTmf8x01Sensor", sensor.Sensor, cg.PollingComponent, i2c.I2CDevice
)

sensor_mode = dfr_tmf8x01_ns.enum("sensor_mode_t")
SENSOR_MODE = {
    'PROXIMITY': sensor_mode.PROXIMITY,
    'DISTANCE': sensor_mode.DISTANCE,
    'COMBINED': sensor_mode.COMBINED,
}
CONF_SENSOR_MODE = "sensor_mode"

calibration_mode = dfr_tmf8x01_ns.enum("calibration_mode_t")
CALIBRATION_MODE = {
    'NONE': calibration_mode.NONE,
    'CALIBRATE': calibration_mode.CALIBRATE,
    'CALIBRATE_AND_ALGO_STATE': calibration_mode.CALIBRATE_AND_ALGO_STATE,
}
CONF_CALIBRATION_MODE = "calibration_mode"

CONF_INTERRUPT_PIN = "interrupt_pin"

CONFIG_SCHEMA = (
    sensor.sensor_schema(
        DfrTmf8x01Sensor,
        unit_of_measurement=UNIT_METER,
        accuracy_decimals=3,
        icon=ICON_ARROW_EXPAND_VERTICAL,
        device_class=DEVICE_CLASS_EMPTY,
        state_class=STATE_CLASS_MEASUREMENT,
    )
    .extend(
        {
            cv.Optional(CONF_SENSOR_MODE): cv.enum(SENSOR_MODE),
            cv.Optional(CONF_CALIBRATION_MODE): cv.enum(CALIBRATION_MODE),
            cv.Optional(CONF_ENABLE_PIN): pins.gpio_output_pin_schema,
            cv.Optional(CONF_INTERRUPT_PIN): pins.gpio_input_pin_schema,
        }
    )
    .extend(cv.polling_component_schema('60s'))
    .extend(i2c.i2c_device_schema(0x41))
)

async def to_code(config):
    var = await sensor.new_sensor(config)
    await cg.register_component(var, config)
    await i2c.register_i2c_device(var, config)

    if CONF_SENSOR_MODE in config:
        sensor_mode = await cg.get_variable(config[CONF_SENSOR_MODE])
        cg.add(var.set_sensor_mode(sensor_mode))
    if CONF_CALIBRATION_MODE in config:
        calibration_mode = await cg.get_variable(config[CONF_CALIBRATION_MODE])
        cg.add(var.set_calibration_mode(calibration_mode))
    if CONF_ENABLE_PIN in config:
        enable_pin = await cg.gpio_pin_expression(config[CONF_ENABLE_PIN])
        cg.add(var.set_enable_pin(enable_pin))
    if CONF_INTERRUPT_PIN in config:
        interrupt_pin = await cg.gpio_pin_expression(config[CONF_INTERRUPT_PIN])
        cg.add(var.set_interrupt_pin(interrupt_pin))
