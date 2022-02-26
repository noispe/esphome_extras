from esphome.components import display, color
import esphome.config_validation as cv
import esphome.codegen as cg
from esphome.const import CONF_ID, CONF_LAMBDA
from ..ui_components import boxbase

DEPENDENCIES = ["ui_components"]
MULTI_CONF = True

template_box_ns = cg.esphome_ns.namespace("template_box")
TemplateBox = template_box_ns.class_("TemplateBox", boxbase.BaseBox)
CONFIG_SCHEMA = boxbase.BOX_SCHEMA.extend({
    cv.GenerateID(): cv.declare_id(TemplateBox),
    cv.Required(CONF_LAMBDA): cv.lambda_,
})


async def to_code(config):
    var = cg.new_Pvariable(config[CONF_ID])
    await boxbase.generate_common_code(var, config)
    drawer = await cg.process_lambda(
        config[CONF_LAMBDA], [
            (display.DisplayBufferRef, "it"),
            (cg.int32, "x"),
            (cg.int32, "y"),
            (cg.int32, "width"),
            (cg.int32, "height"),
            (boxbase.TextAlign, "align"),
            (color.ColorStruct, "fg_color"),
            (color.ColorStruct, "bg_color"),
        ], return_type=cg.void
    )
    cg.add(var.set_drawer(drawer))
