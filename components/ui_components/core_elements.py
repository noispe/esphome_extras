import esphome.config_validation as cv
import esphome.codegen as cg
from esphome.core import ID
from esphome.const import CONF_LAMBDA, CONF_ENTITY_ID
from esphome.components import font, display, color
from .common import ui_components_ns, BaseElement, CONF_FONT, TextAlign, generate_common_code, register_element, ELEMENT_SCHEMA, CONF_TEXT, IMAGE_PROVIDER_SCHEMA, new_image_provider, validate_is_text_source, validate_has_text_or_sensor
import logging

TextElement = ui_components_ns.class_("TextElement", BaseElement)
ImageElement = ui_components_ns.class_("ImageElement", BaseElement)
ShapeElement = ui_components_ns.class_("ShapeElement", BaseElement)
TemplateElement = ui_components_ns.class_("TemplateElement", BaseElement)

CONF_SHAPE = "shape"
CONF_DEFAULT = "default"
CONF_RADIUS = "radius"

TEXT_ELEMENT_SCHEMA = ELEMENT_SCHEMA.extend({
    cv.Optional(CONF_TEXT): cv.templatable(cv.string),
    cv.Optional(CONF_ENTITY_ID): cv.use_id(cg.EntityBase),
    cv.Optional(CONF_DEFAULT): cv.string_strict,
    cv.Optional(CONF_FONT): cv.use_id(font.Font),
})


@register_element("text", TextElement, TEXT_ELEMENT_SCHEMA, validate_has_text_or_sensor)
async def text_element_code(config, type_id):
    var = await generate_common_code(config, type_id)
    if CONF_FONT in config:
        f = await cg.get_variable(config[CONF_FONT])
        cg.add(var.set_font(f))

    if CONF_ENTITY_ID in config:
        s = await cg.get_variable(config[CONF_ENTITY_ID])
        cg.add(var.set_content(s))
    else:
        template_ = await cg.templatable(config[CONF_TEXT], [], cg.std_string)
        cg.add(var.set_content(template_))

    if CONF_DEFAULT in config:
        cg.add(var.set_default(config[CONF_DEFAULT]))

    return var

IMAGE_ELEMENT_SCHEMA = ELEMENT_SCHEMA.extend(IMAGE_PROVIDER_SCHEMA)


@register_element("image", ImageElement, IMAGE_ELEMENT_SCHEMA)
async def image_element_code(config, type_id):
    var = await generate_common_code(config, type_id)
    ip = await new_image_provider(config)
    cg.add(var.set_image(ip))
    return var

ShapeType = ShapeElement.enum("ShapeType", is_class=True)
SHAPE_TYPES = {
    "CIRCLE": ShapeType.CIRCLE,
    "FILLED_CIRCLE": ShapeType.FILLED_CIRCLE,
    "RECTANGLE": ShapeType.RECTANGLE,
    "FILLED_RECTANGLE": ShapeType.FILLED_RECTANGLE,
    "LINE": ShapeType.LINE,
    "ROUND_RECTANGLE": ShapeType.ROUND_RECTANGLE,
    "ROUND_FILLED_RECTANGLE": ShapeType.ROUND_FILLED_RECTANGLE,
}

def validate_can_have_radius_(config):
    if CONF_RADIUS in config:
        if not config[CONF_SHAPE].startsWith("ROUND_RECTANGLE"):
            raise cv.Invalid(f"{CONF_RADIUS} only works for rounded shapes")
    return config

@register_element("shape", ShapeElement, ELEMENT_SCHEMA.extend({
    cv.Required(CONF_SHAPE): cv.one_of(*SHAPE_TYPES, upper=True),
    cv.Optional(CONF_RADIUS): cv.positive_int,
}))
async def shape_element_code(config, type_id):
    var = await generate_common_code(config, type_id)
    cg.add(var.set_shape_type(SHAPE_TYPES[config[CONF_SHAPE]])),
    if CONF_RADIUS in config:
        cg.add(var.set_radius(config[CONF_RADIUS]))
    return var


@register_element("template", TemplateElement, ELEMENT_SCHEMA.extend({
    cv.Required(CONF_LAMBDA): cv.lambda_,
}), validate_can_have_radius_)
async def template_element_code(config, type_id):
    var = await generate_common_code(config, type_id)
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
