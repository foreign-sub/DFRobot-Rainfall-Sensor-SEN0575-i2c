#pragma once

#include "esphome/core/component.h"
#include "esphome/components/i2c/i2c.h"
#include "esphome/components/sensor/sensor.h"
#include "esphome/core/hal.h"

namespace esphome {
namespace dfrobot_sen0575_i2c {

class DFRobotSen0575I2C : public PollingComponent, public i2c::I2CDevice {
 public:
    void set_cumulative_rainfall(sensor::Sensor *cumulative_rainfall) { cumulative_rainfall_ = cumulative_rainfall; }
    void set_rainfall_within_hour(sensor::Sensor *rainfall_within_hour) { rainfall_within_hour_ = rainfall_within_hour; }
    void set_raw_data(sensor::Sensor *raw_data) { raw_data_ = raw_data; }
    void set_working_time(sensor::Sensor *working_time) { working_time_ = working_time; }

    void setup() override;
    void loop() override;
    void update() override;
    void dump_config() override;
    float get_setup_priority() const override;

    std::string getFirmwareVersion_();
    float cumulative_rainfall();
    float rainfall_within_hour();
    uint32_t raw_data();
    float working_time();

    float getRainfall_();
    float getRainfall_(uint8_t hour);
    uint32_t getRawData_();
    float getWorkingTime_();
    uint8_t setRainAccumulatedValue_(float accumulatedValue);
    bool begin_();
    uint8_t readRegister_(uint8_t reg, uint8_t *pBuf, size_t size);
    uint8_t writeRegister_(uint8_t reg, uint8_t *pBuf, size_t size);

 protected:
    uint8_t _deviceAddr = 0x1D;
    sensor::Sensor *cumulative_rainfall_;
    sensor::Sensor *rainfall_within_hour_;
    sensor::Sensor *raw_data_;
    sensor::Sensor *working_time_;
};

} // namespace dfrobot_sen0575_i2c
} // namespace esphome
