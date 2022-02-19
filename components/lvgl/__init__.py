import esphome.config_validation as cv
import esphome.codegen as cg
from esphome.components import display, touchscreen

from esphome.const import CONF_ID

DEPENDENCIES = ["display","touchscreen"]

CONF_TOUCHSCREEN = "touchscreen"
CONF_DISPLAY = "display"

lvgl_ns = cg.esphome_ns.namespace("lvgl")
LVGLComponent = lvgl_ns.class_("LVGLComponent", cg.Component)

CONFIG_SCHEMA = cv.Schema(
    {
        cv.GenerateID(): cv.declare_id(LVGLComponent),
        cv.GenerateID(CONF_DISPLAY): cv.use_id(display.DisplayBuffer),
        cv.GenerateID(CONF_TOUCHSCREEN): cv.use_id(touchscreen.Touchscreen),
    }
).extend(cv.COMPONENT_SCHEMA)

async def to_code(config):
    cg.add_library("lvgl/lvgl", "8.1.0")
    cg.add_build_flag("-DLV_CONF_SKIP")
    cg.add_build_flag("-DLV_MEM_SIZE=49152U")
    cg.add_build_flag("-DLV_HOR_RES_MAX=240")
    cg.add_build_flag("-DLV_VER_RES_MAX=320")
    cg.add_build_flag("-DTFT_ROTATION=0")
    cg.add_build_flag("-DLV_COLOR_DEPTH=8")
    cg.add_build_flag("-DLV_COLOR_16_SWAP=0")
    cg.add_build_flag("-DLV_DRAW_COMPLEX=1")
    cg.add_build_flag("-DLV_USE_USER_DATA=1")
    cg.add_build_flag("-DLV_USE_PNG=1")
    cg.add_build_flag("-DUSE_LV_LOG=1")
    cg.add_global(lvgl_ns.using)
    var = cg.new_Pvariable(config[CONF_ID])

    disp = await cg.get_variable(config[CONF_DISPLAY])
    cg.add(var.set_display(disp))

    touch = await cg.get_variable(config[CONF_TOUCHSCREEN])
    cg.add(var.set_touchscreen(touch))

    await cg.register_component(var, config)
