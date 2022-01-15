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
        cv.Required(CONF_NUM_LEDS): cv.int_range(min=25, max=65535),
        cv.Optional(CONF_WIDTH, default=1): cv.int_range(1, 50),
        cv.Optional(CONF_WEIGHT, default=1): cv.int_range(1, 25),
        cv.Optional(CONF_SPEED, default=3): cv.int_range(1, 25),
    },
)
async def borealis_light_effect_to_code(config, effect_id):
    effect = cg.new_Pvariable(effect_id, config[CONF_NAME])
    cg.add(effect.set_width_factor(config[CONF_WIDTH]))
    cg.add(effect.set_color_weight_preset(config[CONF_WEIGHT]))
    cg.add(effect.set_speed_factor(config[CONF_SPEED]))
    cg.add(effect.set_num_leds(config[CONF_NUM_LEDS]))
    return effect
