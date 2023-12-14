import esphome.codegen as cg
import esphome.config_validation as cv
from esphome.components import display, i2c
from esphome.const import CONF_ID, CONF_LAMBDA, CONF_PAGES

CODEOWNERS = ["@latonita","@nicolaij"]

AUTO_LOAD = ["display"]

DEPENDENCIES = ["i2c"]

st7567s_ns = cg.esphome_ns.namespace("st7567s")
ST7567S = st7567s_ns.class_("ST7567S", cg.PollingComponent, display.DisplayBuffer, i2c.I2CDevice)

CONFIG_SCHEMA = (
    display.FULL_DISPLAY_SCHEMA.extend(
        {
            cv.GenerateID(): cv.declare_id(ST7567S),
        }
    )
    .extend(i2c.i2c_device_schema(0x3F))
    .extend(cv.polling_component_schema("1s"))
)


async def to_code(config):
    var = cg.new_Pvariable(config[CONF_ID])
    await cg.register_component(var, config)
    await i2c.register_i2c_device(var, config)

    if CONF_LAMBDA in config:
        lambda_ = await cg.process_lambda(
            config[CONF_LAMBDA], [(display.DisplayRef, "it")], return_type=cg.void
        )
        cg.add(var.set_writer(lambda_))
