import functools

import json
from esphome import core
from esphome.components import display, image
from esphome.components.font import validate_truetype_file, validate_pillow_installed
import esphome.config_validation as cv
import esphome.codegen as cg
import esphome.cpp_generator as ccpg
from esphome.const import CONF_ID, CONF_SIZE, CONF_RAW_DATA_ID
from esphome.core import CORE, HexInt

DEPENDENCIES = ["display"]
AUTO_LOAD = ["image"]
MULTI_CONF = True


def validate_icons(value):
    if isinstance(value, list):
        value = cv.Schema([cv.string])(value)
    value = cv.Schema([cv.string])(list(value))
    return value


CONF_FONT_FILE = "font_file"
CONF_METADATA_FILE = "metadata_file"
CONF_ICONS = "icons"
CONF_FONT_ID = "font_id"
CONF_PREFIX = "prefix"

icon_provider_ns = cg.esphome_ns.namespace("icon_provider")
IconProvider = icon_provider_ns.class_(
    "IconProvider", cg.Component
)
Icon = icon_provider_ns.struct(
    "Icon"
)

ICON_PROVIDER_SCHEMA = cv.Schema(
    {
        cv.GenerateID(): cv.declare_id(IconProvider),
        cv.Required(CONF_FONT_FILE): validate_truetype_file,
        cv.Required(CONF_METADATA_FILE): cv.file_,
        cv.Required(CONF_ICONS): validate_icons,
        cv.Optional(CONF_SIZE, default=20): cv.int_range(min=1),
        cv.Optional(CONF_PREFIX, default=''): cv.string_strict,
        cv.GenerateID(CONF_RAW_DATA_ID): cv.declare_id(cg.uint8),
    }
)

CONFIG_SCHEMA = cv.All(validate_pillow_installed, ICON_PROVIDER_SCHEMA)


class PlainStaticConstAssignmentExpression(ccpg.AssignmentExpression):
    __slots__ = ()

    def __init__(self, type_, name, rhs):
        super().__init__(type_, "", name, rhs)

    def __str__(self):
        return f"static const {self.type} {self.name} = {self.rhs}"


def static_const_obj(id_, rhs) -> cg.MockObj:
    rhs = cg.safe_exp(rhs)
    obj = cg.MockObj(id_, ".")
    assignment = PlainStaticConstAssignmentExpression(id_.type, id_, rhs)
    CORE.add(assignment)
    CORE.register_variable(id_, obj)
    return obj


def maybe_prefix(base, prefix):
    if prefix == '':
        return base
    return prefix + ':' + base


def make_square(mask, size):
    from PIL import Image

    width, height = mask.size
    square_mask = mask
    if width > height:
        square_mask = Image.new(mask.mode, (size, size), (0))
        top_left = (0, (width - height) // 2)
        square_mask.paste(mask, (0, (width - height) // 2, width, height + (width - height) // 2))
    else:
        square_mask = Image.new(mask.mode, (size, size), (0))
        square_mask.paste(mask, ((height - width) // 2, 0, width + ((height - width) // 2), height))
    return square_mask


async def to_code(config):
    from PIL import ImageFont

    path = CORE.relative_config_path(config[CONF_FONT_FILE])
    metadata_path = CORE.relative_config_path(config[CONF_METADATA_FILE])
    try:
        font = ImageFont.truetype(path, int(config[CONF_SIZE]))
        with open(metadata_path) as fp:
            metadata = json.load(fp)

    except Exception as e:
        raise core.EsphomeError(f"Could not load truetype file {path}: {e}")

    glyphs = {}
    for icon, codepoint in metadata.items():
        cp = chr(int(codepoint))
        glyphs[maybe_prefix(icon, config[CONF_PREFIX])] = cp

    glyph_args = {}
    used_glyphs = []
    lut_initializer = []

    data = []
    for icon in config[CONF_ICONS]:
        icon = maybe_prefix(icon, config[CONF_PREFIX])
        try:
            glyph = glyphs[icon]
        except Exception as e:
            raise core.EsphomeError(f"Glyph for {icon} was not found: {e} in {glyphs.keys()}")
        if glyph in used_glyphs:  # already have it
            continue
        mask = font.getmask(glyph, "1")
        mask = make_square(mask, int(config[CONF_SIZE]))
        width, height = mask.size
        width8 = ((width + 7) // 8) * 8
        glyph_data = [0 for _ in range(height * width8 // 8)]
        for y in range(height):
            for x in range(width):
                if mask.getpixel((x, y)) == (0):
                    continue
                pos = x + y * width8
                glyph_data[pos // 8] |= 0x80 >> (pos % 8)
        glyph_args[icon] = (len(data), width, height)
        data += glyph_data
        used_glyphs.append(icon)

    rhs = [HexInt(x) for x in data]
    prog_arr = cg.progmem_array(config[CONF_RAW_DATA_ID], rhs)

    for glyph in used_glyphs:
        lut_initializer.append(
            cg.StructInitializer(
                Icon,
                ("name", glyph),
                ("width", glyph_args[glyph][1]),
                ("height", glyph_args[glyph][2]),
                (
                    "data",
                    cg.RawExpression(f"{str(prog_arr)} + {str(glyph_args[glyph][0])}"),
                ),
            )
        )

    var = cg.new_Pvariable(config[CONF_ID], lut_initializer)

    await cg.register_component(var, config)
    cg.add_define("USE_ICON_PROVIDER")
