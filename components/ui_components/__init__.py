import esphome.config_validation as cv
import esphome.codegen as cg
from esphome.core import Lambda
from esphome.const import CONF_ID, CONF_TYPE, CONF_LAMBDA
from esphome.components import text_sensor, sensor, binary_sensor, font, image, display, color
from . import common, core_elements, button_element
from .. import icons

DEPENDENCIES = ["display"]
MULTI_CONF = True

UIComponents = common.ui_components_ns.class_("UIComponents")

CONFIG_SCHEMA = cv.Schema({
    cv.GenerateID(): cv.declare_id(UIComponents),
    cv.Required(common.CONF_ELEMENTS): cv.All(
        cv.ensure_list(common.validate_elements), cv.Length(min=1)
    ),
})


async def to_code(config):
    var = cg.new_Pvariable(config[CONF_ID])
    elements = await common.generate_elements(config)
    cg.add(var.set_content(elements))
