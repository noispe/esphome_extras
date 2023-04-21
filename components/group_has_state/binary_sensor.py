import esphome.codegen as cg
import esphome.config_validation as cv
from esphome.components import binary_sensor, sensor, text_sensor, number, select
from esphome.const import (
    CONF_ID,
    CONF_SENSORS
)


DEPENDENCIES = ["binary_sensor","esp32"]

group_has_state_ns = cg.esphome_ns.namespace("group_has_state")
GroupHasState = group_has_state_ns.class_(
    "GroupHasState", binary_sensor.BinarySensor, cg.PollingComponent
)


def validate_can_have_state(value):
    return cv.Any(
        cv.use_id(text_sensor.TextSensor),
        cv.use_id(sensor.Sensor),
        cv.use_id(binary_sensor.BinarySensor),
        cv.use_id(select.Select),
        cv.use_id(number.Number),
    )(value)


CONFIG_SCHEMA = binary_sensor.binary_sensor_schema(GroupHasState).extend({
        cv.GenerateID(): cv.declare_id(GroupHasState),
        cv.Required(CONF_SENSORS): cv.All(cv.ensure_list(validate_can_have_state), cv.Length(min=1))
    }).extend(cv.polling_component_schema("1s"))


async def to_code(config):
    var = cg.new_Pvariable(config[CONF_ID])
    for e in config[CONF_SENSORS]:
        cg.add(var.monitor_state(await cg.get_variable(e)))
    await cg.register_component(var, config)
    return var
