import esphome.codegen as cg
import esphome.config_validation as cv
from esphome import automation
from esphome.components import i2c, sensor
from esphome.const import CONF_ID, UNIT_CENTIMETER, DEVICE_CLASS_WATER, STATE_CLASS_MEASUREMENT, ICON_RULER
import logging


DEPENDENCIES = ['i2c']

ID_HIGH_ADDR = "high_address"
ID_LOW_ADDR = "low_address"

grove_water_level_sensor = cg.esphome_ns.namespace("grove_water_level_sensor")
GroveWaterLevelSensor = grove_water_level_sensor.class_(
    "GroveWaterLevelSensor", sensor.Sensor, cg.PollingComponent
)
CONFIG_SCHEMA = (
    sensor.sensor_schema(
        GroveWaterLevelSensor,
        unit_of_measurement=UNIT_CENTIMETER,
        icon=ICON_RULER,
        accuracy_decimals=1,
        device_class=DEVICE_CLASS_WATER,
        state_class=STATE_CLASS_MEASUREMENT,
    )
    .extend({
            cv.GenerateID(CONF_ID): cv.declare_id(GroveWaterLevelSensor),
            cv.GenerateID(i2c.CONF_I2C_ID): cv.use_id(i2c.I2CBus),
            cv.Optional(ID_HIGH_ADDR, 0x78): cv.i2c_address,    
            cv.Optional(ID_LOW_ADDR, 0x77): cv.i2c_address,
    })
    .extend(cv.polling_component_schema("60s"))
)

async def to_code(config):
    var = await sensor.new_sensor(config)
    await cg.register_component(var, config)

    parent = await cg.get_variable(config[i2c.CONF_I2C_ID])
    cg.add(var.set_i2c_bus(parent))
    cg.add(var.set_high_address(config[ID_HIGH_ADDR]))   
    cg.add(var.set_low_address(config[ID_LOW_ADDR]))   

