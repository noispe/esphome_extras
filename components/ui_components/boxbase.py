from esphome import core
from esphome.components import display, color
import esphome.config_validation as cv
import esphome.codegen as cg
from esphome.const import CONF_ID, CONF_WIDTH, CONF_HEIGHT, CONF_COLOR, CONF_BORDER
from . import UIComponents, ui_components_ns

CONF_X = "x"
CONF_Y = "y"
CONF_ALIGNMENT = "alignment"
CONF_BACKGROUND = "background"
CONF_RENDER = "renderer"

BaseBox = ui_components_ns.class_("BaseBox")
BaseBoxPtr = BaseBox.operator("ptr")
BaseBoxConstPtr = BaseBox.operator("const_ptr")
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
BOX_SCHEMA = cv.Schema(
    {
        cv.Required(CONF_RENDER): cv.use_id(UIComponents),
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


async def generate_common_code(var, config):
    renderer = await cg.get_variable(config[CONF_RENDER])
    cg.add(renderer.add_component(var))
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
