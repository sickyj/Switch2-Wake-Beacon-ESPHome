import esphome.codegen as cg
import esphome.config_validation as cv
from esphome.const import CONF_ID

CODEOWNERS = ["@sickyj"]

switch2_ns = cg.esphome_ns.namespace("switch2")
Switch2Component = switch2_ns.class_("Switch2Component", cg.Component)

# Registering the component makes ESPHome compile switch2.cpp (which includes
# <esp_mac.h> at file scope) and include switch2.h in main.cpp - so lambdas can
# call switch2::spoof_bt_mac() with no local header file.
CONFIG_SCHEMA = cv.Schema(
    {
        cv.GenerateID(): cv.declare_id(Switch2Component),
    }
).extend(cv.COMPONENT_SCHEMA)


async def to_code(config):
    var = cg.new_Pvariable(config[CONF_ID])
    await cg.register_component(var, config)
