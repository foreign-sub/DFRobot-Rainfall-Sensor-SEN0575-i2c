#pragma once

#include "esphome/core/component.h"
#include "esphome/components/i2c/i2c.h"
#include "esphome/components/sensor/sensor.h"
#include "esphome/core/hal.h"

namespace esphome{
namespace dfrobot_sen0575_i2c{

class DFRobotSen0575I2C : public sensor::Sensor, public PollingComponent, public i2c::I2CDevice {
 public:
    void set_cumulative_rainfall(sensor::Sensor *cumulative_rainfall) { cumulative_rainfall_ = cumulative_rainfall; }
    void set_rainfall_within_hour(sensor::Sensor *rainfall_within_hour) { rainfall_within_hour_ = rainfall_within_hour; }
    void set_raw_data(sensor::Sensor *raw_data) { raw_data_ = raw_data; }
    void set_sensor_working_time(sensor::Sensor *sensor_working_time) { sensor_working_time_ = sensor_working_time; }

    void setup() override;
    void loop() override;
    void update() override;
    void dump_config() override;

    String getFirmwareVersion();
    float getRainfall();
    float getRainfall(uint8_t hour);
    uint32_t getRawData();
    float getSensorWorkingTime();
    uint8_t setRainAccumulatedValue(float accumulatedValue);

    uint8_t readRegister(uint8_t reg, void* pBuf, size_t size);
    uint8_t writeRegister(uint8_t reg, void* pBuf, size_t size);

 protected:
    uint8_t _deviceAddr = 0x1D;
    sensor::Sensor *cumulative_rainfall_;
    sensor::Sensor *rainfall_within_hour_;
    sensor::Sensor *raw_data_;
    sensor::Sensor *sensor_working_time_;
};

} // namespace dfrobot_sen0575_i2c
} // namespace esphome
