from esphome.components import text_sensor
import esphome.config_validation as cv
import esphome.codegen as cg
from esphome.const import CONF_ID, ENTITY_CATEGORY_DIAGNOSTIC, ICON_RESTART_ALERT

DEPENDENCIES = ["text_sensor"]

restart_reason_ns = cg.esphome_ns.namespace("restart_reason")
RestartReasonComponent = restart_reason_ns.class_("RestartReasonComponent", text_sensor.TextSensor, cg.Component)

CONFIG_SCHEMA = (
    text_sensor.text_sensor_schema(
        icon=ICON_RESTART_ALERT,
        entity_category=ENTITY_CATEGORY_DIAGNOSTIC,
    ).extend({cv.GenerateID(): cv.declare_id(RestartReasonComponent)})
    .extend(cv.COMPONENT_SCHEMA)
)


async def to_code(config):
    var = await text_sensor.new_text_sensor(config)
    await cg.register_component(var, config)
