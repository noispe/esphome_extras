import esphome.config_validation as cv
import esphome.codegen as cg
from  esphome.components import color, font
from esphome.const import CONF_ID, CONF_COLOR
from . import common, core_elements, button_element

DEPENDENCIES = ["display"]
MULTI_CONF = True

BaseUiComponent = common.ui_components_ns.class_("BaseUiComponent")
std_tuple = cg.std_ns.class_("tuple")

CONFIG_SCHEMA = cv.Schema({
    cv.GenerateID(): cv.declare_id(BaseUiComponent),
    cv.Required(common.CONF_ELEMENTS): cv.All(
        cv.ensure_list(common.validate_elements), cv.Length(min=1)
    ),
    cv.Optional(CONF_COLOR ): cv.use_id(color.ColorStruct),
    cv.Optional(common.CONF_BACKGROUND): cv.use_id(color.ColorStruct),
    cv.Required(common.CONF_FONT): cv.use_id(font.Font),

})

class MoveVariableExpression(cg.Expression):
    __slots__ = ("name")

    def __init__(self, name):
        self.name = name

    def __str__(self):
        return f"std::move({self.name})"


async def to_code(config):
    elements =  await common.generate_elements(config)

    ui = common.ui_components_ns.class_("UIComponents").template(*common.element_types(config))
    var = cg.Pvariable(config[CONF_ID], ui.new(*elements), ui)
    if CONF_COLOR in config:
        cg.add(var.set_default_fg(await cg.get_variable(config[CONF_COLOR])))
    if common.CONF_BACKGROUND in config:
        cg.add(var.set_default_bg(await cg.get_variable(config[common.CONF_BACKGROUND])))
    cg.add(var.set_default_font(await cg.get_variable(config[common.CONF_FONT])))
    for e in elements:
        await cg.register_parented(e, config[CONF_ID])

    return var
