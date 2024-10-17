#ifndef __DFROBOT_RAINFALL_SENSOR_H__
#define __DFROBOT_RAINFALL_SENSOR_H__

#include <Arduino.h>
#include <Wire.h>
#include <esphome/components/i2c/i2c.h>

class DFRobotRainfallSensor : public Component, public PollingComponent, public i2c::I2CDevice {
public:
    #define I2C_REG_PID                            0x00
    #define I2C_REG_VID                            0x02
    #define I2C_REG_VERSION                        0x0A
    #define I2C_REG_TIME_RAINFALL                  0x0C
    #define I2C_REG_CUMULATIVE_RAINFALL            0x10
    #define I2C_REG_RAW_DATA                       0x14
    #define I2C_REG_SYS_TIME                       0x18
    #define I2C_REG_RAIN_HOUR                      0x26
    #define I2C_REG_BASE_RAINFALL                  0x28

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

private:
    uint8_t _deviceAddr = 0x1D;
};

#endif