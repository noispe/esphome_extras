import esphome.codegen as cg
import esphome.config_validation as cv
from esphome import automation
from esphome.components import i2c, sensor, binary_sensor
from esphome.const import CONF_ID, CONF_BRIGHTNESS, ICON_POWER, ENTITY_CATEGORY_DIAGNOSTIC

MULTI_CONF = True

CONF_AXP192_ID = 'axp192_id'
DEPENDENCIES = ['i2c']
AUTOLOAD = ["switch","binary_sensor","sensor","output"]

CONF_POWER_BUTTON = 'power_button_sensor'
CONF_DISABLE_LDOIO0 = 'disable_ldoio0'
CONF_DISABLE_RTC = 'disable_ldo1'
CONF_DISABLE_LDO2 = 'disable_ldo2'
CONF_DISABLE_LDO3 = 'disable_ldo3'
CONF_DISABLE_DCDC1 = 'disable_dcdc1'
CONF_DISABLE_DCDC2 = 'disable_dcdc2'
CONF_DISABLE_DCDC3 = 'disable_dcdc3'
CONF_LDO2_VOLTAGE = 'ldo2_voltage'
CONF_LDO3_VOLTAGE = 'ldo3_voltage'
CONF_DCDC1_VOLTAGE = 'dcdc1_voltage'
CONF_DCDC2_VOLTAGE = 'dcdc2_voltage'
CONF_DCDC3_VOLTAGE = 'dcdc3_voltage'
CONF_LDOIO0_VOLTAGE = 'ldoio0_voltage'
axp192_ns = cg.esphome_ns.namespace('axp192')


CONF_OUTPUT_RTC = 'rtc'
CONF_OUTPUT_LDO2 = 'ldo2'
CONF_OUTPUT_LDO3 = 'ldo3'
CONF_OUTPUT_DCDC1 = 'dcdc1'
CONF_OUTPUT_DCDC2 = 'dcdc2'
CONF_OUTPUT_DCDC3 = 'dcdc3'
CONF_OUTPUT_LDOIO0 = 'ldoio0'

output_pin = axp192_ns.enum('OutputPin', is_class=True)
OUTPUT_PIN = {
    CONF_OUTPUT_RTC: output_pin.OUTPUT_RTC,
    CONF_OUTPUT_LDO2: output_pin.OUTPUT_LDO2,
    CONF_OUTPUT_LDO3: output_pin.OUTPUT_LDO3,
    CONF_OUTPUT_DCDC1: output_pin.OUTPUT_DCDC1,
    CONF_OUTPUT_DCDC2: output_pin.OUTPUT_DCDC2,
    CONF_OUTPUT_DCDC3: output_pin.OUTPUT_DCDC3,
    CONF_OUTPUT_LDOIO0: output_pin.OUTPUT_LDOIO0,
}


Axp192Component = axp192_ns.class_(
    'Axp192Component', i2c.I2CDevice, cg.PollingComponent)

VoffVoltage = axp192_ns.enum('VoffVoltage', is_class=True)
VOFFVOLTAGE = {
    '2600mV': VoffVoltage.VOFF_2600MV,
    '2700mV': VoffVoltage.VOFF_2700MV,
    '2800mV': VoffVoltage.VOFF_2800MV,
    '2900mV': VoffVoltage.VOFF_2900MV,
    '3000mV': VoffVoltage.VOFF_3000MV,
    '3100mV': VoffVoltage.VOFF_3100MV,
    '3200mV': VoffVoltage.VOFF_3200MV,
    '3300mV': VoffVoltage.VOFF_3300MV,
}
CONF_VOFF_VOLTAGE = 'voff_voltage'

ChargeCurrent = axp192_ns.enum('ChargeCurrent', is_class=True)
CHARGECURRENT = {
    '100mA': ChargeCurrent.CHARGE_100MA,
    '190mA': ChargeCurrent.CHARGE_190MA,
    '280mA': ChargeCurrent.CHARGE_280MA,
    '360mA': ChargeCurrent.CHARGE_360MA,
    '450mA': ChargeCurrent.CHARGE_450MA,
    '550mA': ChargeCurrent.CHARGE_550MA,
    '630mA': ChargeCurrent.CHARGE_630MA,
    '700mA': ChargeCurrent.CHARGE_700MA,
}
CONF_CHARGE_CURRENT = 'charge_current'

ChargeVoltage = axp192_ns.enum('ChargeVoltage', is_class=True)
CHARGEVOLTAGE = {
    '4100mV': ChargeVoltage.CHARGE_4100MV,
    '4150mV': ChargeVoltage.CHARGE_4150MV,
    '4200mV': ChargeVoltage.CHARGE_4200MV,
    '4360mV': ChargeVoltage.CHARGE_4360MV,
}
CONF_CHARGE_VOLTAGE = 'charge_voltage'

VBusHoldVoltageLimit = axp192_ns.enum('VBusHoldVoltageLimit', is_class=True)
VBUS_HOLD_VOLTAGE_LIMIT = {
    '4000mV': VBusHoldVoltageLimit.HOLD_4000MV,
    '4100mV': VBusHoldVoltageLimit.HOLD_4100MV,
    '4200mV': VBusHoldVoltageLimit.HOLD_4200MV,
    '4300mV': VBusHoldVoltageLimit.HOLD_4300MV,
    '4400mV': VBusHoldVoltageLimit.HOLD_4400MV,
    '4500mV': VBusHoldVoltageLimit.HOLD_4500MV,
    '4600mV': VBusHoldVoltageLimit.HOLD_4600MV,
    '4700mV': VBusHoldVoltageLimit.HOLD_4700MV,
}
CONF_VBUS_HOLD_VOLTAGE_LIMIT = 'vbus_hold_voltage_limit'

VBusHoldCurrentLimit = axp192_ns.enum('VBusHoldCurrentLimit', is_class=True)
VBUS_HOLD_CURRENT_LIMIT = {
    '500mA': VBusHoldCurrentLimit.CURRENT_LIMIT_500MA,
    '100mA': VBusHoldCurrentLimit.CURRENT_LIMIT_100MA,
}
CONF_VBUS_HOLD_CURRENT_LIMIT = 'vbus_hold_current_limit'

VBusIpsout = axp192_ns.enum('VBusIpsout', is_class=True)
VBUS_IPSOUT = {
    'NO': VBusIpsout.IPSOUT_NOT_MANAGED,
    'YES': VBusIpsout.IPSOUT_MANAGED,
}
CONF_VBUS_IPSOUT = 'vbus_ipsout'

LDOio0Control = axp192_ns.enum('LDOio0Control', is_class=True)
LDOIO0_MODE = {
    'NMOS_OPENDRAIN': LDOio0Control.NMOS_OPENDRAIN,
    'GENERAL_INPUT': LDOio0Control.GENERAL_INPUT,
    'LOWNOISE_LDO': LDOio0Control.LOWNOISE_LDO,
    'ADC_IN': LDOio0Control.ADC_IN,
    'LOW_OUTPUT': LDOio0Control.LOW_OUTPUT,
    'FLOATING': LDOio0Control.FLOATING,

}
CONF_LDOIO0_MODE = 'ldoio0_mode'


PowerOffAction = axp192_ns.class_('PowerOffAction', automation.Action)
PrepareSleepAction = axp192_ns.class_('PrepareSleepAction', automation.Action)

OUTPUT_VOLTAGE_RANGE = {
    CONF_OUTPUT_LDO2: cv.float_range(1.8, 3.3),
    CONF_OUTPUT_LDO3: cv.float_range(1.8, 3.3),
    CONF_OUTPUT_DCDC1: cv.float_range(0.7, 3.5),
    CONF_OUTPUT_DCDC2: cv.float_range(0.7, 2.275),
    CONF_OUTPUT_DCDC3: cv.float_range(0.7, 3.5),
    CONF_OUTPUT_LDOIO0: cv.float_range(1.8, 3.3),
}

CONFIG_SCHEMA = cv.Schema({
    cv.GenerateID(): cv.declare_id(Axp192Component),
    cv.Optional(CONF_VOFF_VOLTAGE): cv.enum(VOFFVOLTAGE),
    cv.Optional(CONF_CHARGE_CURRENT): cv.enum(CHARGECURRENT),
    cv.Optional(CONF_CHARGE_VOLTAGE): cv.enum(CHARGEVOLTAGE),
    cv.Optional(CONF_VBUS_HOLD_VOLTAGE_LIMIT): cv.enum(VBUS_HOLD_VOLTAGE_LIMIT),
    cv.Optional(CONF_VBUS_HOLD_CURRENT_LIMIT): cv.enum(VBUS_HOLD_CURRENT_LIMIT),
    cv.Optional(CONF_VBUS_IPSOUT): cv.enum(VBUS_IPSOUT),
    cv.Optional(CONF_LDOIO0_MODE): cv.enum(LDOIO0_MODE),
    cv.Optional(CONF_DISABLE_LDO2): cv.boolean,
    cv.Optional(CONF_DISABLE_LDO3): cv.boolean,
    cv.Optional(CONF_DISABLE_RTC): cv.boolean,
    cv.Optional(CONF_DISABLE_DCDC1): cv.boolean,
    cv.Optional(CONF_DISABLE_DCDC2): cv.boolean,
    cv.Optional(CONF_DISABLE_DCDC3): cv.boolean,
    cv.Optional(CONF_LDO2_VOLTAGE): cv.All(cv.voltage, OUTPUT_VOLTAGE_RANGE[CONF_OUTPUT_LDO2]),
    cv.Optional(CONF_LDO3_VOLTAGE): cv.All(cv.voltage, OUTPUT_VOLTAGE_RANGE[CONF_OUTPUT_LDO3]),
    cv.Optional(CONF_DCDC1_VOLTAGE): cv.All(cv.voltage, OUTPUT_VOLTAGE_RANGE[CONF_OUTPUT_DCDC1]),
    cv.Optional(CONF_DCDC2_VOLTAGE): cv.All(cv.voltage, OUTPUT_VOLTAGE_RANGE[CONF_OUTPUT_DCDC2]),
    cv.Optional(CONF_DCDC3_VOLTAGE): cv.All(cv.voltage, OUTPUT_VOLTAGE_RANGE[CONF_OUTPUT_DCDC3]),
    cv.Optional(CONF_LDOIO0_VOLTAGE): cv.All(cv.voltage, OUTPUT_VOLTAGE_RANGE[CONF_OUTPUT_LDOIO0]),
}).extend(i2c.i2c_device_schema(0x77)).extend(cv.polling_component_schema('60s'))


async def to_code(config):
    cg.add_global(axp192_ns.using)
    var = cg.new_Pvariable(config[CONF_ID])
    await cg.register_component(var, config)
    await i2c.register_i2c_device(var, config)

    for key in [
        CONF_VOFF_VOLTAGE,
        CONF_CHARGE_CURRENT,
        CONF_CHARGE_VOLTAGE,
        CONF_VBUS_HOLD_VOLTAGE_LIMIT,
        CONF_VBUS_HOLD_CURRENT_LIMIT,
        CONF_VBUS_IPSOUT,
        CONF_LDOIO0_MODE,
        CONF_DISABLE_LDO2,
        CONF_DISABLE_LDO3,
        CONF_DISABLE_RTC,
        CONF_DISABLE_DCDC1,
        CONF_DISABLE_DCDC2,
        CONF_DISABLE_DCDC3,
        CONF_LDO2_VOLTAGE,
        CONF_LDO3_VOLTAGE,
        CONF_DCDC1_VOLTAGE,
        CONF_DCDC2_VOLTAGE,
        CONF_DCDC3_VOLTAGE,
        CONF_LDOIO0_VOLTAGE,
    ]:
        if key not in config:
            continue
        conf = config[key]
        cg.add(getattr(var, f"set_{key}")(config[key]))


@automation.register_action(
    'axp192.power_off',
    PowerOffAction,
    cv.Schema({cv.GenerateID(): cv.use_id(Axp192Component)}),
)
async def axp192_power_off_to_code(config, action_id, args):
    var = cg.new_Pvariable(action_id)
    await cg.register_parented(var, config[CONF_ID])
    return var


@automation.register_action(
    'axp192.prepare_sleep',
    PrepareSleepAction,
    cv.Schema({cv.GenerateID(): cv.use_id(Axp192Component)}),
)
async def axp192_prepare_sleep_to_code(config, action_id, args):
    var = cg.new_Pvariable(action_id)
    await cg.register_parented(var, config[CONF_ID])
    return var
