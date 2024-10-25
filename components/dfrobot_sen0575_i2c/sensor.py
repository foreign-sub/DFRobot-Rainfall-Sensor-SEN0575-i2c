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

dfrobot_sen0575_i2c_ns = cg.esphome_ns.namespace("dfrobot_sen0575_i2c")
DFRobotSen0575I2C = dfrobot_sen0575_i2c_ns.class_(
    "DFRobotSen0575I2C", cg.PollingComponent, i2c.I2CDevice
)

CONF_DFROBOT_SEN0575_ADDRESS = 0x1D
CONF_CUMULATIVE_RAINFALL = "cumulative_rainfall"
CONF_RAINFALL_WITHIN_HOUR = "rainfall_within_hour"
CONF_RAW_DATA = "raw_data"
CONF_WORKING_TIME = "working_time"

UNIT_MILLIMETERS_PER_HOUR = "mm/h"

CONFIG_SCHEMA = cv.All(
    cv.Schema(
        {
            cv.GenerateID(): cv.declare_id(DFRobotSen0575I2C),
            cv.Optional(CONF_CUMULATIVE_RAINFALL): sensor.sensor_schema(
                unit_of_measurement=UNIT_MILLIMETER, accuracy_decimals=2,
                icon=ICON_WATER,
                device_class=DEVICE_CLASS_PRECIPITATION,
                state_class=STATE_CLASS_TOTAL_INCREASING,
            ),
            cv.Optional(CONF_RAINFALL_WITHIN_HOUR): sensor.sensor_schema(
                unit_of_measurement=UNIT_MILLIMETERS_PER_HOUR, accuracy_decimals=2,
                icon=ICON_WATER,
                device_class=DEVICE_CLASS_PRECIPITATION_INTENSITY,
                state_class=STATE_CLASS_MEASUREMENT,
            ),
            cv.Optional(CONF_RAW_DATA): sensor.sensor_schema(
                unit_of_measurement=CONF_COUNT, accuracy_decimals=0,
                device_class=DEVICE_CLASS_PRECIPITATION,
                state_class=STATE_CLASS_MEASUREMENT,
            ),
            cv.Optional(CONF_WORKING_TIME): sensor.sensor_schema(
                unit_of_measurement=CONF_HOUR, accuracy_decimals=0,
                device_class=DEVICE_CLASS_DURATION,
                state_class=STATE_CLASS_MEASUREMENT,
            ),
        }
    )
    .extend(cv.polling_component_schema("60s"))
    .extend(i2c.i2c_device_schema(CONF_DFROBOT_SEN0575_ADDRESS))
)

FINAL_VALIDATE_SCHEMA = i2c.final_validate_device_schema(
    "dfrobot_sen0575_i2c",
    min_frequency=50000.0,
    max_frequency=400000.0,
)


async def to_code(config):
    var = cg.new_Pvariable(config[CONF_ID])
    await cg.register_component(var, config)
    await i2c.register_i2c_device(var, config)

    if cumulative_rainfall_config := config.get(CONF_CUMULATIVE_RAINFALL):
        sens = await sensor.new_sensor(cumulative_rainfall_config)
        cg.add(var.set_cumulative_rainfall(sens))

    if rainfall_within_hour_config := config.get(CONF_RAINFALL_WITHIN_HOUR):
        sens = await sensor.new_sensor(rainfall_within_hour_config)
        cg.add(var.set_rainfall_within_hour(sens))

    if raw_data_config := config.get(CONF_RAW_DATA):
        sens = await sensor.new_sensor(raw_data_config)
        cg.add(var.set_raw_data(sens))

    if working_time_config := config.get(CONF_WORKING_TIME):
        sens = await sensor.new_sensor(working_time_config)
        cg.add(var.set_working_time(sens))
