import esphome.codegen as cg
import esphome.config_validation as cv
from esphome.components.light.types import AddressableLightEffect
from esphome.components.light.effects import register_addressable_effect
from esphome.const import CONF_NAME, CONF_WIDTH, CONF_SPEED, CONF_WEIGHT, CONF_NUM_LEDS

borealis_ns = cg.esphome_ns.namespace("borealis")
BorealisLightEffect = borealis_ns.class_("BorealisLightEffect", AddressableLightEffect)

CONFIG_SCHEMA = cv.Schema({})


@register_addressable_effect(
    "borealis",
    BorealisLightEffect,
    "Borealis Effect",
    {
        cv.Required(CONF_NUM_LEDS): cv.uint8_t,
        cv.Optional(CONF_WIDTH): cv.int_range(1, 10),
        cv.Optional(CONF_WEIGHT): cv.int_range(1, 10),
        cv.Optional(CONF_SPEED): cv.int_range(1, 10),
    },
)
async def borealis_light_effect_to_code(config, effect_id):
    effect = cg.new_Pvariable(effect_id, config[CONF_NAME])
    if config[CONF_WIDTH] != None:
        effect.set_width_factor(config[CONF_WIDTH])
    if config[CONF_WEIGHT] != None:
        effect.set_color_weight_preset(config[CONF_WEIGHT])
    if config[CONF_SPEED] != None:
        effect.set_speed_factor(config[CONF_SPEED])
    effect.set_num_leds(config[CONF_NUM_LEDS])
    cg.add(effect)
    return effect
