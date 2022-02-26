from esphome import core
from esphome.components import image
import esphome.config_validation as cv
import esphome.codegen as cg
from esphome.const import CONF_ID
from ..ui_components import boxbase

DEPENDENCIES = ["ui_components"]
MULTI_CONF = True

CONF_IMAGE = "image"

imagebox_ns = cg.esphome_ns.namespace("imagebox")
ImageBox = imagebox_ns.class_("ImageBox", boxbase.BaseBox)
ImagePtr = image.Image_.operator("ptr")
CONFIG_SCHEMA = boxbase.BOX_SCHEMA.extend({
    cv.GenerateID(): cv.declare_id(ImageBox),
    cv.Required(CONF_IMAGE): cv.templatable(cv.use_id(image.Image_))
})

async def locate_image(config):
    return await cg.get_variable(config[CONF_IMAGE])

async def to_code(config):
    var = cg.new_Pvariable(config[CONF_ID])
    await boxbase.generate_common_code(var, config)
    if isinstance(config[CONF_IMAGE], core.Lambda):
        template_ = await cg.templatable(config[CONF_IMAGE], [], ImagePtr)
        cg.add(var.set_image(template_))
    else:
        img = await cg.get_variable(config[CONF_IMAGE])
        cg.add(var.set_image(img))
