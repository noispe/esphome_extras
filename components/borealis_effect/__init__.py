from esphome.jsonschema import jschema_extractor
import esphome.codegen as cg
import esphome.config_validation as cv
from esphome import automation
from esphome.const import CONF_ID

ledeffect_ns = cg.esphome_ns.namespace("ledeffect")
LedEffect = ledeffect_ns.class_(
    "LedEffect", cg.PollingComponent
)

LedEffect = cg.esphome_ns.struct("Color")

CONFIG_SCHEMA = cv.Schema(
    {
        cv.Required(CONF_ID): cv.declare_id(ColorStruct),

    }
).extend(cv.COMPONENT_SCHEMA)


async def to_code(config):
    var = cg.new_Pvariable(config[CONF_ID])
    await cg.register_component(var, config)

