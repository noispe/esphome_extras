from esphome import core
from esphome.components import font, text_sensor
from esphome.const import CONF_ID
import esphome.config_validation as cv
import esphome.codegen as cg
from ..ui_components import boxbase

MULTI_CONF = True
DEPENDENCIES = ["font", "ui_components", "text_sensor"]

CONF_TEXT = "text"
CONF_TEXT_SENSOR = "text_sensor"
CONF_FONT = "font"

textbox_ns = cg.esphome_ns.namespace("textbox")
TextBox = textbox_ns.class_("TextBox", boxbase.BaseBox)
CONFIG_SCHEMA = boxbase.BOX_SCHEMA.extend({
    cv.GenerateID(): cv.declare_id(TextBox),
    cv.Exclusive(CONF_TEXT, "text"): cv.templatable(cv.string),
    cv.Exclusive(CONF_TEXT_SENSOR, "text"): cv.use_id(text_sensor.TextSensor),
    cv.Required(CONF_FONT): cv.use_id(font.Font),
})


async def to_code(config):
    var = cg.new_Pvariable(config[CONF_ID])
    await boxbase.generate_common_code(var, config)
    f = await cg.get_variable(config[CONF_FONT])
    cg.add(var.set_font(f))
    if CONF_TEXT_SENSOR in config:
        s = await cg.get_variable(config[CONF_TEXT_SENSOR])
        cg.add(var.set_content(s))
    else:
        template_ = await cg.templatable(config[CONF_TEXT], [], cg.std_string)
        cg.add(var.set_content(template_))
