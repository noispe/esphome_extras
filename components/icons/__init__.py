import functools

from esphome import core
from esphome.components import display
from esphome.components.font import Font, Glyph, GlyphData, CONF_RAW_GLYPH_ID, validate_truetype_file, validate_pillow_installed
import esphome.config_validation as cv
import esphome.codegen as cg
import esphome.cpp_generator as ccpg
from esphome.const import CONF_ID, CONF_SIZE, CONF_RAW_DATA_ID
from esphome.core import CORE, HexInt
import json

DEPENDENCIES = ["font"]
MULTI_CONF = True

def validate_icons(value):
    if isinstance(value, list):
        value = cv.Schema([cv.string])(value)
    value = cv.Schema([cv.string])(list(value))
    return value

CONF_FONT_FILE = "font_file"
CONF_METADATA_FILE = "metadata_file"
CONF_ICON_LUT_ID = "icon_lookup_lut"
CONF_ICONS = "icons"
CONF_FONT_ID = "font_id"
CONF_PREFIX = "prefix"

icon_provider_ns = cg.esphome_ns.namespace("icon_provider")
IconProvider = icon_provider_ns.class_(
    "IconProvider", cg.Component
)

ICON_PROVIDER_SCHEMA = cv.Schema(
    {
        cv.Required(CONF_ID): cv.declare_id(IconProvider),
        cv.Required(CONF_FONT_FILE): validate_truetype_file,
        cv.Required(CONF_METADATA_FILE): cv.file_,
        cv.Required(CONF_ICONS): validate_icons,
        cv.Optional(CONF_SIZE, default=20): cv.int_range(min=1),
        cv.Optional(CONF_PREFIX, default=''): cv.string_strict,
        cv.GenerateID(CONF_RAW_DATA_ID): cv.declare_id(cg.uint8),
        cv.GenerateID(CONF_RAW_GLYPH_ID): cv.declare_id(GlyphData),
        cv.GenerateID(CONF_ICON_LUT_ID): cv.declare_id(cg.RawStatement('std::unordered_map<std::string,const char*>')),
        cv.GenerateID(CONF_FONT_ID): cv.declare_id(Font),

    }
)

CONFIG_SCHEMA = cv.All(validate_pillow_installed, ICON_PROVIDER_SCHEMA)

class PlainStaticConstAssignmentExpression(ccpg.AssignmentExpression):
    __slots__ = ()

    def __init__(self, type_, name, rhs, obj):
        super().__init__(type_, "", name, rhs, obj)

    def __str__(self):
        return f"static const {self.type} {self.name} = {self.rhs}"

def static_const_obj(id_, rhs) -> "MockObj":
    rhs = cg.safe_exp(rhs)
    obj = cg.MockObj(id_, ".")
    assignment = PlainStaticConstAssignmentExpression(id_.type, id_, rhs, obj)
    CORE.add(assignment)
    CORE.register_variable(id_, obj)
    return obj

def maybe_prefix(base, prefix):
    if prefix == '':
        return base
    return prefix + ':' + base

async def to_code(config):
    from PIL import ImageFont

    path = CORE.relative_config_path(config[CONF_FONT_FILE])
    metadata_path = CORE.relative_config_path(config[CONF_METADATA_FILE])
    try:
        font = ImageFont.truetype(path, config[CONF_SIZE])
        with open(metadata_path) as fp:
            metadata = json.load(fp)

    except Exception as e:
        raise core.EsphomeError(f"Could not load truetype file {path}: {e}")

    glyphs = {}
    for icon in metadata: # get primary icons
        cp = chr(int(icon['codepoint'],16))
        glyphs[maybe_prefix(icon['name'],config[CONF_PREFIX])] = cp

    for icon in metadata: # fill in aliases later because the might overlap with primary
        cp = chr(int(icon['codepoint'],16))
        for alias in icon['aliases']:
            alias = maybe_prefix(alias,config[CONF_PREFIX])
            if alias not in glyphs.keys():
                glyphs[alias] = cp

    ascent, descent = font.getmetrics()

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
        lut_initializer.append([icon,glyph])
        if glyph in used_glyphs: #already have it
            continue
        mask = font.getmask(glyph, mode="1")
        _, (offset_x, offset_y) = font.font.getsize(glyph)
        width, height = mask.size
        width8 = ((width + 7) // 8) * 8
        glyph_data = [0] * (height * width8 // 8)
        for y in range(height):
            for x in range(width):
                if not mask.getpixel((x, y)):
                    continue
                pos = x + y * width8
                glyph_data[pos // 8] |= 0x80 >> (pos % 8)
        glyph_args[glyph] = (len(data), offset_x, offset_y, width, height)
        data += glyph_data
        used_glyphs.append(glyph)

    rhs = [HexInt(x) for x in data]
    prog_arr = cg.progmem_array(config[CONF_RAW_DATA_ID], rhs)

    glyph_initializer = []
    for glyph in used_glyphs:
        glyph_initializer.append(
            cg.StructInitializer(
                GlyphData,
                ("a_char", glyph),
                (
                    "data",
                    cg.RawExpression(f"{str(prog_arr)} + {str(glyph_args[glyph][0])}"),
                ),
                ("offset_x", glyph_args[glyph][1]),
                ("offset_y", glyph_args[glyph][2]),
                ("width", glyph_args[glyph][3]),
                ("height", glyph_args[glyph][4]),
            )
        )

    glyph_bytes = cg.static_const_array(config[CONF_RAW_GLYPH_ID], glyph_initializer)
    lut = static_const_obj(config[CONF_ICON_LUT_ID], lut_initializer)

    font = cg.new_Pvariable(
        config[CONF_FONT_ID], glyph_bytes, len(glyph_initializer), ascent, ascent + descent
    )
    var = cg.new_Pvariable(config[CONF_ID], font, lut)

    await cg.register_component(var, config)
