#include "sensor.h"

DFRobotRainfallSensor::DFRobotRainfallSensor(i2c::I2CDevice *parent, uint8_t address) : i2c::I2CDevice(parent), PollingComponent(15000) {
    _deviceAddr = address;
}

void DFRobotRainfallSensor::setup() {
    if (!begin()) {
        ESP_LOGE("DFRobotRainfallSensor", "Failed to initialize sensor");
    }
}

void DFRobotRainfallSensor::update() {
    // Read cumulative rainfall
    uint8_t buff = {0};
    readRegister(I2C_REG_CUMULATIVE_RAINFALL, buff, 4);
    uint32_t rainfall = (buff | (buff << 8) | (buff << 16) | (buff << 24));
    publish_state(rainfall / 10000.0f);
}

float DFRobotRainfallSensor::getRainfall() {
    uint8_t buff = {0};
    readRegister(I2C_REG_CUMULATIVE_RAINFALL, buff, 4);
    uint32_t rainfall = (buff | (buff << 8) | (buff << 16) | (buff << 24));
    return rainfall / 10000.0f;
}

float DFRobotRainfallSensor::getRainfall(uint8_t hour) {
    writeRegister(I2C_REG_RAIN_HOUR, &hour, 1);
    uint8_t buff = {0};
    if (readRegister(I2C_REG_TIME_RAINFALL, buff, 4) == 0) {
        return -1;
    }
    uint32_t rainfall = (buff | (buff << 8) | (buff << 16) | (buff << 24));
    return rainfall / 10000.0f;
}

uint32_t DFRobotRainfallSensor::getRawData() {
    uint8_t buff = {0};
    readRegister(I2C_REG_RAW_DATA, buff, 4);
    uint32_t rawdata = (buff | (buff << 8) | (buff << 16) | (buff << 24));
    return rawdata;
}

float DFRobotRainfallSensor::getSensorWorkingTime() {
    uint8_t buff = {0};
    readRegister(I2C_REG_SYS_TIME, buff, 2);
    uint16_t workingTime = (buff | (buff << 8));
    return workingTime / 60.0f;
}

uint8_t DFRobotRainfallSensor::setRainAccumulatedValue(float accumulatedValue) {
    uint16_t data = accumulatedValue * 10000;
    uint8_t buff = {data & 0xFF, data >> 8};
    return writeRegister(I2C_REG_BASE_RAINFALL, buff, 2);
}

bool DFRobotRainfallSensor::begin() {
    uint8_t buff = {0};
    readRegister(I2C_REG_PID, buff, 4);
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