from esphome import core
import esphome.config_validation as cv
import esphome.codegen as cg
from esphome.const import CONF_ID, CONF_TYPE
from ..ui_components import boxbase

DEPENDENCIES = ["ui_components"]
MULTI_CONF = True

shapebox_ns = cg.esphome_ns.namespace("shapebox")
ShapeBox = shapebox_ns.class_("ShapeBox", boxbase.BaseBox)
ShapeType = ShapeBox.enum("ShapeType", is_class=True)
SHAPE_TYPES = {
    "CIRCLE": ShapeType.CIRCLE,
    "FILLED_CIRCLE": ShapeType.FILLED_CIRCLE,
    "RECTANGLE": ShapeType.RECTANGLE,
    "FILLED_RECTANGLE": ShapeType.FILLED_RECTANGLE,
    "LINE": ShapeType.LINE,
}
CONFIG_SCHEMA = boxbase.BOX_SCHEMA.extend({
    cv.GenerateID(): cv.declare_id(ShapeBox),
    cv.Required(CONF_TYPE): cv.one_of(*SHAPE_TYPES, upper=True),
})

async def to_code(config):
    var = cg.new_Pvariable(config[CONF_ID])
    await boxbase.generate_common_code(var, config)
    cg.add(var.set_shape_type(SHAPE_TYPES[config[CONF_TYPE]]))
