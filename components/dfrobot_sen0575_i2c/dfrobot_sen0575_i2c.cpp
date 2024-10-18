#include "dfrobot_sen0575_i2c.h"
#include "esphome/core/log.h"
#include "esphome/core/hal.h"

namespace esphome{
namespace dfrobot_sen0575_i2c{

static const char *const TAG = "dfrobot_sen0575_i2c";

DFRobotRainfallSensor::DFRobotRainfallSensor(i2c::I2CDevice *parent, uint8_t address) : i2c::I2CDevice(parent), PollingComponent(15000) {
    _deviceAddr = address;
}

void DFRobotRainfallSensor::setup() {
    if (!begin()) {
        ESP_LOGE("DFRobotRainfallSensor", "Failed to initialize sensor");
    }
    ESP_LOGCONFIG(TAG, "Setting up SEN0575...");
}

void DFRobotRainfallSensor::update() {
    // Read cumulative rainfall
    uint8_t buff = {0};
    readRegister(Registers::CUMULATIVE_RAINFALL, buff, 4);
    uint32_t rainfall = (buff | (buff << 8) | (buff << 16) | (buff << 24));
    publish_state(rainfall / 10000.0f);
}

float DFRobotRainfallSensor::getRainfall() {
    uint8_t buff = {0};
    readRegister(Registers::CUMULATIVE_RAINFALL, buff, 4);
    uint32_t rainfall = (buff | (buff << 8) | (buff << 16) | (buff << 24));
    return rainfall / 10000.0f;
}

float DFRobotRainfallSensor::getRainfall(uint8_t hour) {
    writeRegister(Registers::RAIN_HOUR, &hour, 1);
    uint8_t buff = {0};
    if (readRegister(Registers::TIME_RAINFALL, buff, 4) == 0) {
        return -1;
    }
    uint32_t rainfall = (buff | (buff << 8) | (buff << 16) | (buff << 24));
    return rainfall / 10000.0f;
}

uint32_t DFRobotRainfallSensor::getRawData() {
    uint8_t buff = {0};
    readRegister(Registers::RAW_DATA, buff, 4);
    uint32_t rawdata = (buff | (buff << 8) | (buff << 16) | (buff << 24));
    return rawdata;
}

float DFRobotRainfallSensor::getSensorWorkingTime() {
    uint8_t buff = {0};
    readRegister(Registers::SYS_TIME, buff, 2);
    uint16_t workingTime = (buff | (buff << 8));
    return workingTime / 60.0f;
}

uint8_t DFRobotRainfallSensor::setRainAccumulatedValue(float accumulatedValue) {
    uint16_t data = accumulatedValue * 10000;
    uint8_t buff = {data & 0xFF, data >> 8};
    return writeRegister(Registers::BASE_RAINFALL, buff, 2);
}

bool DFRobotRainfallSensor::begin() {
    uint8_t buff = {0};
    readRegister(Registers::PID, buff, 4);
    uint32_t pid = (buff | (buff << 8) | (buff << 16) | (buff << 24));
    uint32_t vid = (buff | (buff & 0x3F) << 8);
    return (vid == 0x3343 && pid == 0x100C0);
}

uint8_t DFRobotRainfallSensor::readRegister(uint8_t reg, void* pBuf, size_t size) {
    this->write(this->_deviceAddr, reg);
    if (this->read(this->_deviceAddr, pBuf, size) != size) {
        return 0;
    }
    return size;
}

uint8_t DFRobotRainfallSensor::writeRegister(uint8_t reg, void* pBuf, size_t size) {
    this->write(this->_deviceAddr, reg, pBuf, size);
    delay(100);
    return 0;
}

} // namespace dfrobot_sen0575_i2c
} // namespace esphome
