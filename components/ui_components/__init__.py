import esphome.config_validation as cv
import esphome.codegen as cg
from esphome.core import Lambda
from esphome.const import CONF_ID, CONF_TYPE, CONF_LAMBDA
from esphome.components import text_sensor, sensor, binary_sensor, font, image, display, color
from .common import ui_components_ns, BaseElement, CONF_ELEMENTS, TextAlign, generate_common_code, generate_elements, validate_elements, register_element
from .. import icons

DEPENDENCIES = ["display"]
MULTI_CONF = True

UIComponents = ui_components_ns.class_("UIComponents")
TextElement = ui_components_ns.class_("TextElement", BaseElement)
ImageElement = ui_components_ns.class_("ImageElement", BaseElement)
ShapeElement = ui_components_ns.class_("ShapeElement", BaseElement)
TemplateElement = ui_components_ns.class_("TemplateElement", BaseElement)
ImagePtr = image.Image_.operator("ptr")

CONF_SENSOR = "sensor"
CONF_BINARY_SENSOR = "sensor"
CONF_TEXT = "text"
CONF_TEXT_SENSOR = "text_sensor"
CONF_FONT = "font"
CONF_IMAGE = "image"
CONF_SHAPE = "shape"
CONF_DEFAULT = "default"

CONFIG_SCHEMA = cv.Schema({
    cv.GenerateID(): cv.declare_id(UIComponents),
    cv.Required(CONF_ELEMENTS): cv.All(
        cv.ensure_list(validate_elements), cv.Length(min=1)
    ),
})


async def to_code(config):
    var = cg.new_Pvariable(config[CONF_ID])
    elements = await generate_elements(config)
    cg.add(var.set_content(elements))


@register_element("text", TextElement, {
    cv.Exclusive(CONF_TEXT, "text"): cv.templatable(cv.string),
    cv.Exclusive(CONF_TEXT_SENSOR, "text"): cv.use_id(text_sensor.TextSensor),
    cv.Exclusive(CONF_SENSOR, "text"): cv.use_id(sensor.Sensor),
    cv.Exclusive(CONF_BINARY_SENSOR, "text"): cv.use_id(binary_sensor.BinarySensor),
    cv.Required(CONF_FONT): cv.use_id(font.Font),
    cv.Optional(CONF_DEFAULT): cv.string_strict,
})
async def text_element_code(config, type_id):
    var = cg.new_Pvariable(type_id)
    await generate_common_code(var, config)
    f = await cg.get_variable(config[CONF_FONT])
    cg.add(var.set_font(f))
    if CONF_DEFAULT in config:
        cg.add(var.set_default(config[CONF_DEFAULT]))
    if CONF_TEXT_SENSOR in config:
        s = await cg.get_variable(config[CONF_TEXT_SENSOR])
        cg.add(var.set_content(s))
    elif CONF_SENSOR in config:
        s = await cg.get_variable(config[CONF_SENSOR])
        cg.add(var.set_content(s))
    elif CONF_BINARY_SENSOR in config:
        s = await cg.get_variable(config[CONF_BINARY_SENSOR])
        cg.add(var.set_content(s))
    else:
        template_ = await cg.templatable(config[CONF_TEXT], [], cg.std_string)
        cg.add(var.set_content(template_))
    return var


@register_element("image", ImageElement, {
    cv.Exclusive(CONF_IMAGE , "image"): cv.templatable(cv.use_id(image.Image_)),
    cv.Exclusive(CONF_TEXT_SENSOR, "image"): cv.Schema({
        cv.Required(icons.CONF_ICONS): cv.use_id(icons.IconProvider),
        cv.Required(CONF_SENSOR): cv.use_id(text_sensor.TextSensor),
    })
})

async def image_element_code(config, type_id):
    var = cg.new_Pvariable(type_id)
    await generate_common_code(var, config)
    if CONF_TEXT_SENSOR in config:
        icns = await cg.get_variable(config[CONF_TEXT_SENSOR][icons.CONF_ICONS])
        text = await cg.get_variable(config[CONF_TEXT_SENSOR][CONF_SENSOR])
        cg.add(var.set_image(text,icns))
    else:
        if isinstance(config[CONF_IMAGE], Lambda):
            template_ = await cg.templatable(config[CONF_IMAGE], [], ImagePtr)
            cg.add(var.set_image(template_))
        else:
            img = await cg.get_variable(config[CONF_IMAGE])
            cg.add(var.set_image(img))
    return var

ShapeType = ShapeElement.enum("ShapeType", is_class=True)
SHAPE_TYPES = {
    "CIRCLE": ShapeType.CIRCLE,
    "FILLED_CIRCLE": ShapeType.FILLED_CIRCLE,
    "RECTANGLE": ShapeType.RECTANGLE,
    "FILLED_RECTANGLE": ShapeType.FILLED_RECTANGLE,
    "LINE": ShapeType.LINE,
}


@register_element("shape", ShapeElement, {
    cv.Required(CONF_SHAPE): cv.one_of(*SHAPE_TYPES, upper=True),
})
async def shape_element_code(config, type_id):
    var = cg.new_Pvariable(type_id)
    await generate_common_code(var, config)
    cg.add(var.set_shape_type(SHAPE_TYPES[config[CONF_SHAPE]]))
    return var


@register_element("template", TemplateElement, {
    cv.Required(CONF_LAMBDA): cv.lambda_,
})
async def template_element_code(config, type_id):
    var = cg.new_Pvariable(type_id)
    await generate_common_code(var, config)
    drawer = await cg.process_lambda(
        config[CONF_LAMBDA], [
            (display.DisplayBufferRef, "it"),
            (cg.int32, "x"),
            (cg.int32, "y"),
            (cg.int32, "width"),
            (cg.int32, "height"),
            (TextAlign, "align"),
            (color.ColorStruct, "fg_color"),
            (color.ColorStruct, "bg_color"),
        ], return_type=cg.void
    )
    cg.add(var.set_drawer(drawer))
    return var
