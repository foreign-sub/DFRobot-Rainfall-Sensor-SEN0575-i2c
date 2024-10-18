import esphome.codegen as cg
import esphome.config_validation as cv
from esphome.components import i2c, sensor
from esphome.const import (
    CONF_COUNT,
    CONF_HOUR,
    CONF_ID,
    DEVICE_CLASS_DURATION,
    DEVICE_CLASS_PRECIPITATION_INTENSITY,
    DEVICE_CLASS_PRECIPITATION,
    STATE_CLASS_MEASUREMENT,
    STATE_CLASS_TOTAL_INCREASING,
    UNIT_MILLIMETER,
    ICON_WATER,
)

CODEOWNERS = ["@foreign-sub"]
DEPENDENCIES = ["i2c"]
MULTI_CONF = True

dfrobot_sen0575_ns = cg.esphome_ns.namespace("dfrobot_sen0575_i2c")
DFRobotSen0575Component = dfrobot_sen0575_ns.class_(
    "DFRobotSen0575Component", cg.Component, i2c.I2CDevice
)

CONF_DFROBOT_SEN0575_ID = "dfrobot_sen0575_id"
CONF_DFROBOT_SEN0575_ADDRESS = 0x1D
CONF_CUMULATIVE_RAINFALL = "cumulative_rainfall"
CONF_RAINFALL_WITHIN_HOUR = "rainfall_within_hour"
CONF_RAW_DATA = "raw_data"
CONF_SENSOR_WORKING_TIME = "sensor_working_time"

UNIT_MILLIMETERS_PER_HOUR = "mm/h"

CONFIG_SCHEMA = cv.All(
    cv.Schema(
        {
            cv.GenerateID(): cv.declare_id(DFRobotSen0575Component),
            cv.Optional(CONF_CUMULATIVE_RAINFALL): sensor.sensor_schema(
                unit_of_measurement=UNIT_MILLIMETER,
                accuracy_decimals=2,
                icon=ICON_WATER,
                device_class=DEVICE_CLASS_PRECIPITATION,
                state_class=STATE_CLASS_TOTAL_INCREASING,
            ),
            cv.Optional(CONF_RAINFALL_WITHIN_HOUR): sensor.sensor_schema(
                unit_of_measurement=UNIT_MILLIMETERS_PER_HOUR,
                accuracy_decimals=2,
                icon=ICON_WATER,
                device_class=DEVICE_CLASS_PRECIPITATION_INTENSITY,
                state_class=STATE_CLASS_MEASUREMENT,
            ),
            cv.Optional(CONF_RAW_DATA): sensor.sensor_schema(
                unit_of_measurement=CONF_COUNT,
                accuracy_decimals=0,
                device_class=DEVICE_CLASS_PRECIPITATION,
                state_class=STATE_CLASS_MEASUREMENT,
            ),
            cv.Optional(CONF_SENSOR_WORKING_TIME): sensor.sensor_schema(
                unit_of_measurement=CONF_HOUR,
                accuracy_decimals=0,
                device_class=DEVICE_CLASS_DURATION,
                state_class=STATE_CLASS_MEASUREMENT,
            ),
        }
    )
    .extend(cv.COMPONENT_SCHEMA)
    .extend(i2c.i2c_device_schema(CONF_DFROBOT_SEN0575_ADDRESS))
)

async def to_code(config):
    var = cg.new_Pvariable(config[CONF_ID])
    await cg.register_component(var, config)
    await i2c.register_i2c_device(var, config)
