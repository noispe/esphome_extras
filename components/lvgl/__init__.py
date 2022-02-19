import esphome.config_validation as cv
import esphome.codegen as cg
from esphome.components import color

lvgl_ns = cg.esphome_ns.namespace("lvgl")
LVGLComponent = select_ns.class_("LVGLComponent", cg.Component)

CONFIG_BASE_COMPONENT_SCHEMA = cv.Schema(
    {
        v.GenerateID(): cv.declare_id(LVGLComponent),
    }
).extend(cv.COMPONENT_SCHEMA)

@coroutine_with_priority(40.0)
async def to_code(config):
    var = cg.new_Pvariable(config[CONF_ID])
    await cg.register_component(var, config)
