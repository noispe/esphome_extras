from esphome.components import i2c, light
import esphome.config_validation as cv
import esphome.codegen as cg
from esphome.const import CONF_OUTPUT_ID

AUTO_LOAD = ['light']
DEPENDENCIES = ['i2c']

yunhat_ns = cg.esphome_ns.namespace('yunhat')
YunHatLights = yunhat_ns.class_('YunHatLights', light.AddressableLight, cg.Component, i2c.I2CDevice)

CONFIG_SCHEMA = cv.All(light.ADDRESSABLE_LIGHT_SCHEMA.extend({
    cv.GenerateID(CONF_OUTPUT_ID): cv.declare_id(YunHatLights)
}).extend(cv.COMPONENT_SCHEMA).extend(i2c.i2c_device_schema(0x38)))

def to_code(config):
    var = cg.new_Pvariable(config[CONF_OUTPUT_ID])
    yield cg.register_component(var, config)
    yield i2c.register_i2c_device(var, config)
    yield light.register_light(var, config)


