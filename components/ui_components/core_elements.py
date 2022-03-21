import esphome.config_validation as cv
import esphome.codegen as cg
from esphome.core import Lambda
from esphome.const import CONF_ID, CONF_TYPE, CONF_LAMBDA
from esphome.components import text_sensor, sensor, binary_sensor, font, image, display, color
from .common import ui_components_ns, BaseElement, TextAlign, generate_common_code, register_element, ELEMENT_SCHEMA, ICON_SCHEMA, CONF_SENSOR
from .. import icons

TextElement = ui_components_ns.class_("TextElement", BaseElement)
ImageElement = ui_components_ns.class_("ImageElement", BaseElement)
ShapeElement = ui_components_ns.class_("ShapeElement", BaseElement)
TemplateElement = ui_components_ns.class_("TemplateElement", BaseElement)
ImagePtr = image.Image_.operator("ptr")

CONF_BINARY_SENSOR = "sensor"
CONF_TEXT = "text"
CONF_TEXT_SENSOR = "text_sensor"
CONF_FONT = "font"
CONF_IMAGE = "image"
CONF_SHAPE = "shape"
CONF_DEFAULT = "default"


@register_element("text", TextElement,
                  ELEMENT_SCHEMA.extend({
                      cv.Optional(CONF_TEXT): cv.templatable(cv.string),
                      cv.Optional(CONF_TEXT_SENSOR): cv.use_id(text_sensor.TextSensor),
                      cv.Optional(CONF_SENSOR): cv.use_id(sensor.Sensor),
                      cv.Optional(CONF_BINARY_SENSOR): cv.use_id(binary_sensor.BinarySensor),
                      cv.Optional(CONF_DEFAULT): cv.string_strict,
                      cv.Required(CONF_FONT): cv.use_id(font.Font),
                  }),
                  cv.has_at_most_one_key(CONF_TEXT, CONF_TEXT_SENSOR, CONF_SENSOR, CONF_BINARY_SENSOR)
                  )
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


@register_element("image", ImageElement,
                  ELEMENT_SCHEMA.extend({
                      cv.Optional(CONF_IMAGE): cv.templatable(cv.use_id(image.Image_)),
                      cv.Optional(CONF_TEXT_SENSOR): ICON_SCHEMA
                  }),
                  cv.has_at_most_one_key(CONF_IMAGE, CONF_TEXT_SENSOR)
                  )
async def image_element_code(config, type_id):
    var = cg.new_Pvariable(type_id)
    await generate_common_code(var, config)
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

ShapeType = ShapeElement.enum("ShapeType", is_class=True)
SHAPE_TYPES = {
    "CIRCLE": ShapeType.CIRCLE,
    "FILLED_CIRCLE": ShapeType.FILLED_CIRCLE,
    "RECTANGLE": ShapeType.RECTANGLE,
    "FILLED_RECTANGLE": ShapeType.FILLED_RECTANGLE,
    "LINE": ShapeType.LINE,
}


@register_element("shape", ShapeElement, ELEMENT_SCHEMA.extend({
    cv.Required(CONF_SHAPE): cv.one_of(*SHAPE_TYPES, upper=True),
}))
async def shape_element_code(config, type_id):
    var = cg.new_Pvariable(type_id)
    await generate_common_code(var, config)
    cg.add(var.set_shape_type(SHAPE_TYPES[config[CONF_SHAPE]]))
    return var


@register_element("template", TemplateElement, ELEMENT_SCHEMA.extend({
    cv.Required(CONF_LAMBDA): cv.lambda_,
}))
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
