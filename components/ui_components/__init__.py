import esphome.config_validation as cv
import esphome.codegen as cg
from esphome.const import CONF_ID

DEPENDENCIES = ["display"]
MULTI_CONF = True

ui_components_ns = cg.esphome_ns.namespace("ui_components")
UIComponents = ui_components_ns.class_("UIComponents")
CONFIG_SCHEMA = cv.Schema({
    cv.GenerateID(): cv.declare_id(UIComponents),
})


async def to_code(config):
    cg.new_Pvariable(config[CONF_ID])
