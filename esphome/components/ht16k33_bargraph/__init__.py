from esphome import automation
import esphome.codegen as cg
import esphome.config_validation as cv
from esphome.components import i2c
from esphome.const import CONF_ID

MULTI_CONF = True
DEPENDENCIES = ["i2c"]

CONF_BRIGHTNESS = "brightness"
CONF_COLOR = "color"
CONF_BAR = "bar"
CONF_FROM_BAR = "from_bar"
CONF_TO_BAR = "to_bar"

COLORS = {
    "off":    0,
    "red":    1,
    "yellow": 2,
    "green":  3,
}

ht16k33_bargraph_ns = cg.esphome_ns.namespace("ht16k33_bargraph")
HT16K33Bargraph = ht16k33_bargraph_ns.class_("HT16K33Bargraph", cg.Component, i2c.I2CDevice)

ClearAction        = ht16k33_bargraph_ns.class_("ClearAction",        automation.Action)
WriteDisplayAction = ht16k33_bargraph_ns.class_("WriteDisplayAction", automation.Action)
FillAction         = ht16k33_bargraph_ns.class_("FillAction",         automation.Action)
SetBarAction       = ht16k33_bargraph_ns.class_("SetBarAction",       automation.Action)
FillRangeAction    = ht16k33_bargraph_ns.class_("FillRangeAction",    automation.Action)
SetBrightnessAction = ht16k33_bargraph_ns.class_("SetBrightnessAction", automation.Action)

CONFIG_SCHEMA = (
    cv.Schema(
        {
            cv.GenerateID(): cv.declare_id(HT16K33Bargraph),
            cv.Optional(CONF_BRIGHTNESS, default=15): cv.int_range(min=0, max=15),
        }
    )
    .extend(cv.COMPONENT_SCHEMA)
    .extend(i2c.i2c_device_schema(0x70))
)


async def to_code(config):
    var = cg.new_Pvariable(config[CONF_ID])
    await cg.register_component(var, config)
    await i2c.register_i2c_device(var, config)
    cg.add(var.set_initial_brightness(config[CONF_BRIGHTNESS]))


# ---------------------------------------------------------------------------
# Actions
# ---------------------------------------------------------------------------

@automation.register_action(
    "ht16k33_bargraph.clear",
    ClearAction,
    cv.maybe_simple_value({cv.Required(CONF_ID): cv.use_id(HT16K33Bargraph)}, key=CONF_ID),
)
async def clear_action_to_code(config, action_id, template_arg, args):
    paren = await cg.get_variable(config[CONF_ID])
    return cg.new_Pvariable(action_id, template_arg, paren)


@automation.register_action(
    "ht16k33_bargraph.write_display",
    WriteDisplayAction,
    cv.maybe_simple_value({cv.Required(CONF_ID): cv.use_id(HT16K33Bargraph)}, key=CONF_ID),
)
async def write_display_action_to_code(config, action_id, template_arg, args):
    paren = await cg.get_variable(config[CONF_ID])
    return cg.new_Pvariable(action_id, template_arg, paren)


@automation.register_action(
    "ht16k33_bargraph.fill",
    FillAction,
    cv.Schema(
        {
            cv.Required(CONF_ID): cv.use_id(HT16K33Bargraph),
            cv.Required(CONF_COLOR): cv.templatable(cv.enum(COLORS, lower=True)),
        }
    ),
)
async def fill_action_to_code(config, action_id, template_arg, args):
    paren = await cg.get_variable(config[CONF_ID])
    var = cg.new_Pvariable(action_id, template_arg, paren)
    color = await cg.templatable(config[CONF_COLOR], args, cg.uint8)
    cg.add(var.set_color(color))
    return var


@automation.register_action(
    "ht16k33_bargraph.set_bar",
    SetBarAction,
    cv.Schema(
        {
            cv.Required(CONF_ID): cv.use_id(HT16K33Bargraph),
            cv.Required(CONF_BAR): cv.templatable(cv.int_range(min=0, max=23)),
            cv.Required(CONF_COLOR): cv.templatable(cv.enum(COLORS, lower=True)),
        }
    ),
)
async def set_bar_action_to_code(config, action_id, template_arg, args):
    paren = await cg.get_variable(config[CONF_ID])
    var = cg.new_Pvariable(action_id, template_arg, paren)
    bar = await cg.templatable(config[CONF_BAR], args, cg.uint8)
    cg.add(var.set_bar(bar))
    color = await cg.templatable(config[CONF_COLOR], args, cg.uint8)
    cg.add(var.set_color(color))
    return var


@automation.register_action(
    "ht16k33_bargraph.fill_range",
    FillRangeAction,
    cv.Schema(
        {
            cv.Required(CONF_ID): cv.use_id(HT16K33Bargraph),
            cv.Required(CONF_FROM_BAR): cv.templatable(cv.int_range(min=0, max=23)),
            cv.Required(CONF_TO_BAR): cv.templatable(cv.int_range(min=0, max=23)),
            cv.Required(CONF_COLOR): cv.templatable(cv.enum(COLORS, lower=True)),
        }
    ),
)
async def fill_range_action_to_code(config, action_id, template_arg, args):
    paren = await cg.get_variable(config[CONF_ID])
    var = cg.new_Pvariable(action_id, template_arg, paren)
    from_bar = await cg.templatable(config[CONF_FROM_BAR], args, cg.uint8)
    cg.add(var.set_from_bar(from_bar))
    to_bar = await cg.templatable(config[CONF_TO_BAR], args, cg.uint8)
    cg.add(var.set_to_bar(to_bar))
    color = await cg.templatable(config[CONF_COLOR], args, cg.uint8)
    cg.add(var.set_color(color))
    return var


@automation.register_action(
    "ht16k33_bargraph.set_brightness",
    SetBrightnessAction,
    cv.Schema(
        {
            cv.Required(CONF_ID): cv.use_id(HT16K33Bargraph),
            cv.Required(CONF_BRIGHTNESS): cv.templatable(cv.int_range(min=0, max=15)),
        }
    ),
)
async def set_brightness_action_to_code(config, action_id, template_arg, args):
    paren = await cg.get_variable(config[CONF_ID])
    var = cg.new_Pvariable(action_id, template_arg, paren)
    brightness = await cg.templatable(config[CONF_BRIGHTNESS], args, cg.uint8)
    cg.add(var.set_brightness(brightness))
    return var
