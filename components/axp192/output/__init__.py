import esphome.codegen as cg
import esphome.config_validation as cv
from esphome.components import output
from esphome.const import CONF_ID, CONF_VOLTAGE, CONF_POWER
from .. import axp192_ns, Axp192Component, CONF_AXP192_ID, OUTPUT_PIN, OUTPUT_VOLTAGE_RANGE

DEPENDENCIES = ["axp192", "output"]

Axp192Output = axp192_ns.class_("Axp192Output", output.FloatOutput)
CONF_OUTPUT = "output"
TYPE_SCHEMAS = dict([
    key,
    output.FLOAT_OUTPUT_SCHEMA.extend(
    {
        cv.Required(CONF_ID): cv.declare_id(Axp192Output),
        cv.GenerateID(CONF_AXP192_ID): cv.use_id(Axp192Component),
        cv.Optional(CONF_OUTPUT): cv.one_of(OUTPUT_VOLTAGE_RANGE.keys()),
        cv.Optional(CONF_POWER, "power"): cv.percentage,
        #cv.Exclusive(CONF_POWER, "power"): cv.percentage,
        #cv.Exclusive(CONF_VOLTAGE, "power"): cv.All(cv.voltage,value)
    }
    ).extend(cv.COMPONENT_SCHEMA)
] for key, value in OUTPUT_VOLTAGE_RANGE.items())

print(TYPE_SCHEMAS.keys())

CONFIG_SCHEMA = cv.typed_schema(
    TYPE_SCHEMAS,
    lower=True,
    key=CONF_OUTPUT
)

async def to_code(config):
    var = cg.new_Pvariable(config[CONF_ID])
    await cg.register_parented(var, config[CONF_AXP192_ID])
    cg.add(var.set_output(OUTPUT_PIN[config[CONF_OUTPUT]]))
    if CONF_POWER in config:
        cg.add(var.set_level(config[CONF_POWER]))
    if CONF_VOLTAGE in config:
        cg.add(var.set_voltage(config[CONF_VOLTAGE]))
    await output.register_output(var, config)
    return var
