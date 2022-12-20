import esphome.config_validation as cv
import esphome.codegen as cg
from esphome.components import touchscreen, binary_sensor, button, switch
from .common import ui_components_ns, register_element, validate_has_text_or_sensor
from .core_elements import TextElement, text_element_code, TEXT_ELEMENT_SCHEMA, ImageElement, image_element_code, IMAGE_ELEMENT_SCHEMA

DEPENDENCIES = ["touchscreen"]

AbstractButton = ui_components_ns.class_("AbstractButton", touchscreen.TouchListener)
TextButtonElement = ui_components_ns.class_("TextButtonElement", AbstractButton, TextElement)
IconButtonElement = ui_components_ns.class_("IconButtonElement", AbstractButton, ImageElement)

CONF_CONTROL = "control"


def validate_is_button_source(value):
    return cv.Any(
        cv.use_id(binary_sensor.BinarySensor),
        cv.use_id(button.Button),
        cv.use_id(switch.Switch),
    )(value)


BUTTON_SCHEMA = cv.Schema({
    cv.Required(touchscreen.CONF_TOUCHSCREEN_ID): cv.use_id(touchscreen.Touchscreen),
    cv.Required(CONF_CONTROL): validate_is_button_source,
})


async def generate_common_button_code(button, config):
    touch = await cg.get_variable(config[touchscreen.CONF_TOUCHSCREEN_ID])
    cg.add(button.set_touchscreen(touch))
    b = await cg.get_variable(config[CONF_CONTROL])
    cg.add(button.set_control(b))
    cg.add_define("USE_TOUCHSCREEN")


@register_element("text_button", TextButtonElement, TEXT_ELEMENT_SCHEMA.extend(BUTTON_SCHEMA), validate_has_text_or_sensor)
async def text_button_element_code(config, type_id):
    var = await text_element_code(config, type_id)
    await generate_common_button_code(var, config)
    return var


@register_element("icon_button", IconButtonElement, IMAGE_ELEMENT_SCHEMA.extend(BUTTON_SCHEMA))
async def icon_button_element_code(config, type_id):
    var = await image_element_code(config, type_id)
    await generate_common_button_code(var, config)
    return var
