from esphome.components import display, color, text_sensor, image, text_sensor, sensor, binary_sensor, select, number
from esphome.core import ID, CORE
import esphome.config_validation as cv
import esphome.codegen as cg
from esphome.util import Registry
from esphome.const import CONF_TYPE_ID, CONF_WIDTH, CONF_HEIGHT, CONF_COLOR, CONF_BORDER, CONF_ENTITY_ID, CONF_NAME
from .. import icons
import logging

_LOGGER = logging.getLogger(__name__)

CONF_X = "x"
CONF_Y = "y"
CONF_ALIGNMENT = "alignment"
CONF_BACKGROUND = "background"
CONF_ELEMENTS = "elements"
CONF_IMAGE = "image"
CONF_ICON = "icon"
CONF_TEXT = "text"
CONF_FONT = "font"

ui_components_ns = cg.esphome_ns.namespace("ui_components")
BaseElement = ui_components_ns.class_("BaseElement")
BaseElementPtr = BaseElement.operator("ptr")
BaseElementConstPtr = BaseElement.operator("const_ptr")
ImageProvider = ui_components_ns.class_("ImageProvider")
ImagePtr = image.Image_.operator("ptr")

TextAlign = display.display_ns.enum("TextAlign", is_class=True)
ALIGNMENTS = {
    "TOP": TextAlign.TOP,
    "CENTER_VERTICAL": TextAlign.CENTER_VERTICAL,
    "BASELINE": TextAlign.BASELINE,
    "BOTTOM": TextAlign.BOTTOM,
    "LEFT": TextAlign.LEFT,
    "CENTER_HORIZONTAL": TextAlign.CENTER_HORIZONTAL,
    "RIGHT": TextAlign.RIGHT,
    "TOP_LEFT": TextAlign.TOP_LEFT,
    "TOP_CENTER": TextAlign.TOP_CENTER,
    "TOP_RIGHT": TextAlign.TOP_RIGHT,
    "CENTER_LEFT": TextAlign.CENTER_LEFT,
    "CENTER": TextAlign.CENTER,
    "CENTER_RIGHT": TextAlign.CENTER_RIGHT,
    "BASELINE_LEFT": TextAlign.BASELINE_LEFT ,
    "BASELINE_CENTER": TextAlign.BASELINE_CENTER,
    "BASELINE_RIGHT": TextAlign.BASELINE_RIGHT,
    "BOTTOM_LEFT": TextAlign.BOTTOM_LEFT,
    "BOTTOM_CENTER": TextAlign.BOTTOM_CENTER,
    "BOTTOM_RIGHT": TextAlign.BOTTOM_RIGHT,
}



def validate_is_text_source(value):
    return cv.Any(
        cv.use_id(text_sensor.TextSensor),
        cv.use_id(sensor.Sensor),
        cv.use_id(binary_sensor.BinarySensor),
        cv.use_id(select.Select),
        cv.use_id(number.Number),
    )(value)

def validate_has_text_or_sensor(config):
    if CONF_TEXT in config and CONF_ENTITY_ID in config:
        raise cv.Invalid(f"only the '{CONF_TEXT}' or '{CONF_ENTITY_ID}' the property can be defined")
    if CONF_TEXT in config or CONF_ENTITY_ID in config:
        return config
    raise cv.Invalid(f"either the '{CONF_TEXT}' or the '{CONF_ENTITY_ID}' property must be defined")

ELEMENT_SCHEMA = cv.Schema(
    {
        cv.Required(CONF_X): cv.int_range(min=0),
        cv.Required(CONF_Y): cv.int_range(min=0),
        cv.Optional(CONF_WIDTH): cv.int_range(min=1),
        cv.Optional(CONF_HEIGHT): cv.int_range(min=1),
        cv.Optional(CONF_ALIGNMENT, default="TOP_LEFT"): cv.one_of(
            *ALIGNMENTS, upper=True
        ),
        cv.Optional(CONF_COLOR): cv.use_id(color.ColorStruct),
        cv.Optional(CONF_BACKGROUND): cv.use_id(color.ColorStruct),
        cv.Optional(CONF_BORDER, default=False): cv.boolean,
    }
)

ICON_SCHEMA = cv.All(cv.Schema({
    cv.Required(icons.CONF_ICONS): cv.use_id(icons.IconProvider),
    cv.Optional(CONF_TEXT): cv.templatable(cv.string),
    cv.Optional(CONF_ENTITY_ID): validate_is_text_source,
}), validate_has_text_or_sensor)

IMAGE_PROVIDER_SCHEMA = cv.Schema({
    cv.Exclusive(CONF_IMAGE, "image"): cv.templatable(cv.use_id(image.Image_)),
    cv.Exclusive(CONF_ICON, "image"): ICON_SCHEMA
})

def to_img(val):
    return cg.RawExpression(f"{val}")


async def new_image_provider(config):
    if CONF_ICON in config:  # icon
        icnconfig = config[CONF_ICON]
        ip = await cg.get_variable(icnconfig[icons.CONF_ICONS])
        if CONF_NAME in icnconfig:
            template_ = await cg.templatable(icnconfig[CONF_NAME], [], cg.std_string)
            return ImageProvider.new(template_, ip)
        else:
            ts = await cg.get_variable(icnconfig[CONF_ENTITY_ID])
            return ImageProvider.new(ts, ip)
    else:  # image
        template_ = await cg.templatable(config[CONF_IMAGE], [], ImagePtr, to_exp=to_img)
        return ImageProvider.new(template_)


ELEMENTS_REGISTRY = Registry()


def register_element(name, type, schema, *kwargs):
    schema = cv.All(cv.Schema({cv.GenerateID(): cv.declare_id(type)}).extend(schema), *kwargs)
    _LOGGER.debug(f"Register {name}")
    return ELEMENTS_REGISTRY.register(name, type, schema)


validate_elements = cv.validate_registry_entry(CONF_ELEMENTS, ELEMENTS_REGISTRY)


async def generate_elements(config):
    elements = await cg.build_registry_list(
        ELEMENTS_REGISTRY, config[CONF_ELEMENTS]
    )
    return elements


def element_types(config):
    cells = [d[CONF_TYPE_ID].type.operator("ptr") for d in config[CONF_ELEMENTS]]
    _LOGGER.info(cells)

    return cells


class VariableDeclarationExpression(cg.Expression):
    __slots__ = ("type", "modifier", "name")

    def __init__(self, type_, modifier, name):
        self.type = type_
        self.modifier = modifier
        self.name = name

    def __str__(self):
        return f"{self.type} {self.modifier}{self.name}"


class AssignmentExpression(cg.Expression):
    __slots__ = ("type", "modifier", "name", "rhs")

    def __init__(self, type_, modifier, name, rhs):
        self.type = type_
        self.modifier = modifier
        self.name = name
        self.rhs = cg.safe_exp(rhs)

    def __str__(self):
        if self.type is None:
            return f"{self.name} = {self.rhs}"
        return f"{self.type} {self.modifier}{self.name} = {self.rhs}"


def new_local_variable(id_: ID) -> cg.MockObj:
    obj = cg.MockObj(id_, ".")
    decl = VariableDeclarationExpression(id_.type, "", id_)
    cg.add(decl)
    CORE.register_variable(id_, obj)
    return obj

def new_local_Pvariable(id_: ID) -> cg.MockObj:
    obj = cg.MockObj(id_, "->")
    decl = AssignmentExpression(id_.type.operator("ptr"), "", id_, id_.type.new())
    cg.add(decl)
    CORE.register_variable(id_, obj)
    return obj


async def generate_common_code(config, type_id):
    var = new_local_Pvariable(type_id)
    #var = cg.new_Pvariable(type_id);
    cg.add(var.set_x(config[CONF_X]))
    cg.add(var.set_y(config[CONF_Y]))
    cg.add(var.set_alignment(ALIGNMENTS[config[CONF_ALIGNMENT]]))
    cg.add(var.set_border(config[CONF_BORDER]))

    if CONF_WIDTH in config:
        cg.add(var.set_width(config[CONF_WIDTH]))
    if CONF_HEIGHT in config:
        cg.add(var.set_height(config[CONF_HEIGHT]))
    if CONF_COLOR in config:
        col = await cg.get_variable(config[CONF_COLOR])
        cg.add(var.set_fg_color(col))
    if CONF_BACKGROUND in config:
        col = await cg.get_variable(config[CONF_BACKGROUND])
        cg.add(var.set_bg_color(col))
    return var
