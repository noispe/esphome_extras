import esphome.config_validation as cv
import esphome.codegen as cg
from esphome.core import Lambda
from esphome.const import CONF_ID, CONF_TYPE, CONF_LAMBDA
from esphome.components import touchscreen, display, binary_sensor, font
from .common import ui_components_ns, BaseElement, generate_common_code, register_element
from .. import icons

ButtonElement = ui_components_ns.class_("ButtonElement", BaseElement, touchscreen.TouchListener)

CONF_SENSOR = "sensor_id"
CONF_TEXT = "text"
CONF_DISPLAY = "display_id"
CONF_FONT = "font"

@register_element("button", ButtonElement, {
    cv.Required(touchscreen.CONF_TOUCHSCREEN_ID): cv.use_id(touchscreen.Touchscreen),
    cv.Required(CONF_SENSOR): cv.use_id(binary_sensor.BinarySensor),
    cv.Required(CONF_FONT): cv.use_id(font.Font),
    cv.GenerateID(CONF_DISPLAY): cv.use_id(display.DisplayBuffer),
    cv.Optional(CONF_TEXT): cv.string_strict
})
async def button_element_code(config, type_id):
    var = cg.new_Pvariable(type_id)
    await generate_common_code(var, config)
    f = await cg.get_variable(config[CONF_FONT])
    cg.add(var.set_font(f))
    touch = await cg.get_variable(config[touchscreen.CONF_TOUCHSCREEN_ID])
    cg.add(var.set_touchscreen(touch))

    button = await cg.get_variable(config[CONF_SENSOR])
    cg.add(var.set_button(button))

    disp = await cg.get_variable(config[CONF_DISPLAY])
    cg.add(var.set_display(disp))

    if CONF_TEXT in config:
        cg.add(var.set_override_text(config[CONF_TEXT]))
    return var
