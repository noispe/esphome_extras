import esphome.config_validation as cv
import esphome.codegen as cg
from esphome.core import Lambda
from esphome.const import CONF_ID, CONF_TYPE, CONF_LAMBDA
from esphome.components import touchscreen, display, binary_sensor, text_sensor, font, image
from .common import ui_components_ns, BaseElement, generate_common_code, register_element, ELEMENT_SCHEMA, ICON_SCHEMA, CONF_SENSOR
from .. import icons

ButtonElement = ui_components_ns.class_("ButtonElement", BaseElement, touchscreen.TouchListener)
TextButtonElement = ui_components_ns.class_("TextButtonElement", ButtonElement)
IconButtonElement = ui_components_ns.class_("IconButtonElement", ButtonElement)

CONF_TEXT = "text"
CONF_DISPLAY = "display_id"
CONF_FONT = "font"
CONF_IMAGE = "image"
CONF_TEXT_SENSOR = "text_sensor"

BUTTON_SCHEMA = ELEMENT_SCHEMA.extend({
    cv.Required(touchscreen.CONF_TOUCHSCREEN_ID): cv.use_id(touchscreen.Touchscreen),
    cv.Required(CONF_SENSOR): cv.use_id(binary_sensor.BinarySensor),
    cv.GenerateID(CONF_DISPLAY): cv.use_id(display.DisplayBuffer),
})


async def generate_common_button_code(button, config):
    touch = await cg.get_variable(config[touchscreen.CONF_TOUCHSCREEN_ID])
    cg.add(button.set_touchscreen(touch))

    sensor = await cg.get_variable(config[CONF_SENSOR])
    cg.add(button.set_button(sensor))

    disp = await cg.get_variable(config[CONF_DISPLAY])
    cg.add(button.set_display(disp))


@register_element("text_button", TextButtonElement, BUTTON_SCHEMA.extend({
    cv.Required(CONF_FONT): cv.use_id(font.Font),
    cv.Optional(CONF_TEXT): cv.string_strict
}))
async def text_button_element_code(config, type_id):
    var = cg.new_Pvariable(type_id)
    await generate_common_code(var, config)
    await generate_common_button_code(var, config)
    f = await cg.get_variable(config[CONF_FONT])
    cg.add(var.set_font(f))

    if CONF_TEXT in config:
        cg.add(var.set_override_text(config[CONF_TEXT]))
    return var


@register_element("icon_button", IconButtonElement,
                  BUTTON_SCHEMA.extend({
                      cv.Optional(CONF_IMAGE): cv.templatable(cv.use_id(image.Image_)),
                      cv.Optional(CONF_TEXT_SENSOR): ICON_SCHEMA
                  }),
                  cv.has_at_most_one_key(CONF_IMAGE, CONF_TEXT_SENSOR)
                  )
async def icon_button_element_code(config, type_id):
    var = cg.new_Pvariable(type_id)
    await generate_common_code(var, config)
    await generate_common_button_code(var, config)
    if CONF_TEXT_SENSOR in config:
        icns = await cg.get_variable(config[CONF_TEXT_SENSOR][icons.CONF_ICONS])
        text = await cg.get_variable(config[CONF_TEXT_SENSOR][CONF_SENSOR])
        cg.add(var.set_image(text, icns))
    else:
        if isinstance(config[CONF_IMAGE], Lambda):
            template_ = await cg.templatable(config[CONF_IMAGE], [], ImagePtr)
            cg.add(var.set_image(template_))
        else:
            img = await cg.get_variable(config[CONF_IMAGE])
            cg.add(var.set_image(img))
    return var
