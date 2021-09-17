import esphome.codegen as cg
import esphome.config_validation as cv
from esphome.components import display, font, image
from esphome import core, automation
from esphome.automation import maybe_simple_id

from esphome.const import (
    CONF_ID
)

DEPENDENCIES = ["display"]
MULTI_CONF = True

CONF_COMMANDS = "commands"
View = display_ns.class_("View")

VIEW_SCHEMA = cv.Schema(
    {
        cv.GenerateID(): cv.declare_id(View),
        cv.Required(CONF_COMMANDS): cv.All(
            cv.ensure_list(cv.string),
            cv.Length(min=1)
        )
    }
)

"""
view:
    id: foo
    commands:
        - set_x(0)
        - set_y(0)
        - println("foo: {}", id("heater"))
        - draw_hline()
"""

async def to_code(config):
    var = cg.new_Pvariable(config[CONF_ID])
    commands_ = [cg.RawExpression("cursor->" + str(x) + ";") for x in config[CONF_ARGS]]
    lambda_ = await cg.process_lambda(
            "\n".join(commands_),
            [(View, "cursor")],
            return_type=cg.void
        )
    cg.add(var.set_render_hook(lambda_))
    await cg.register_component(var, config)
