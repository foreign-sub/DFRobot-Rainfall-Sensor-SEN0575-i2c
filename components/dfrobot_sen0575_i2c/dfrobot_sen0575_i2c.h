#pragma once

#include "esphome/core/component.h"
#include "esphome/components/i2c/i2c.h"
#include "esphome/components/sensor/sensor.h"

namespace esphome{
namespace dfrobot_sen0575_i2c{

class DFRobotRainfallSensor : public Component, public PollingComponent, public i2c::I2CDevice {
public:
    enum class Registers : uint8_t {
        PID = 0x00,
        VID = 0x02,
        VERSION = 0x0A,
        TIME_RAINFALL = 0x0C,
        CUMULATIVE_RAINFALL = 0x10,
        RAW_DATA = 0x14,
        SYS_TIME = 0x18,
        RAIN_HOUR = 0x26,
        BASE_RAINFALL = 0x28
    };

    DFRobotRainfallSensor(i2c::I2CDevice *parent, uint8_t address);
    ~DFRobotRainfallSensor(){};

    void setup() override;
    void update() override;

    float getRainfall();
    float getRainfall(uint8_t hour);
    uint32_t getRawData();
    float getSensorWorkingTime();
    uint8_t setRainAccumulatedValue(float accumulatedValue);

    uint8_t readRegister(uint8_t reg, void* pBuf, size_t size);
    uint8_t writeRegister(uint8_t reg, void* pBuf, size_t size);

protected:
    uint8_t _deviceAddr = 0x1D;
};

#endif

} // namespace dfrobot_sen0575_i2c
} // namespace esphome
