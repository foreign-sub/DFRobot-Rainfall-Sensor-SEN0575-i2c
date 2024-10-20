#include "dfrobot_sen0575_i2c.h"
#include "esphome/core/helpers.h"
#include "esphome/core/log.h"

namespace esphome{
namespace dfrobot_sen0575_i2c{

static const uint8_t REG_PID = 0x00;
static const uint8_t REG_VID = 0x02;
static const uint8_t REG_VERSION = 0x0A;
static const uint8_t REG_TIME_RAINFALL = 0x0C;
static const uint8_t REG_CUMULATIVE_RAINFALL = 0x10;
static const uint8_t REG_RAW_DATA = 0x14;
static const uint8_t REG_SYS_TIME = 0x18;
static const uint8_t REG_RAIN_HOUR = 0x26;
static const uint8_t REG_BASE_RAINFALL = 0x28;

static const char *const TAG = "dfrobot_sen0575_i2c.sensor";

void DFRobotSen0575I2C::setup() {
    ESP_LOGCONFIG(TAG, "Setting up DFRobot SEN0575 '%s'...", this->name_.c_str());
    if (!begin()) {
        ESP_LOGE("DFRobotSen0575I2C", "Failed to initialize sensor");
        this->mark_failed();
        return;
    }
    ESP_LOGCONFIG(TAG, "DFRobot SEN0575 '%s'(Firmware: %s) setup complete.", this->name_.c_str(), this->getFirmwareVersion().c_str());
}

void DFRobotSen0575I2C::loop() {}

void DFRobotSen0575I2C::update() {
    if (this->cumulative_rainfall_ != nullptr)
      this->cumulative_rainfall_->publish_state(this->getRainfall());
    if (this->rainfall_within_hour_ != nullptr)
      this->rainfall_within_hour_->publish_state(this->getRainfall(1));
    if (this->raw_data_ != nullptr)
      this->raw_data_->publish_state(this->getRawData());
    if (this->sensor_working_time_ != nullptr)
      this->sensor_working_time_->publish_state(this->getSensorWorkingTime());
}

void DFRobotSen0575I2C::dump_config(){
    ESP_LOGCONFIG(TAG, "DFRobot Sen0575 I2C '%s':", this->name_.c_str());
    LOG_I2C_DEVICE(this);
    if (this->is_failed()) {
        ESP_LOGE(TAG, "Communication with DFRobot SEN0575 '%s' failed!", this->name_.c_str());
    }
    LOG_UPDATE_INTERVAL(this);
    LOG_SENSOR("  ", "Cumulative Rainfall", this->cumulative_rainfall_);
    LOG_SENSOR("  ", "Rainfall Within Hour", this->rainfall_within_hour_);
    LOG_SENSOR("  ", "Raw Data", this->raw_data_);
    LOG_SENSOR("  ", "Sensor Working Time", this->sensor_working_time_);
}

String DFRobotSen0575I2C::getFirmwareVersion(void)
{
    uint16_t version = 0;
    uint8_t buff[2] = {0};
    readRegister(REG_VERSION, (void*)buff, 2);
    version = buff[0] | ( ((uint16_t)buff[1]) << 8 );
    return String( version >> 12 ) + '.' + String( ( ( version >> 8 ) & 0x0F ) ) + '.' + String( ( ( version >> 4 ) & 0x0F ) ) + '.' + String( ( version & 0x0F ) );
}

float DFRobotSen0575I2C::getRainfall() {
    uint8_t buff = {0};
    readRegister(REG_CUMULATIVE_RAINFALL, buff, 4);
    uint32_t rainfall = (buff | (buff << 8) | (buff << 16) | (buff << 24));
    return rainfall / 10000.0f;
}

float DFRobotSen0575I2C::getRainfall(uint8_t hour) {
    writeRegister(REG_RAIN_HOUR, &hour, 1);
    uint8_t buff = {0};
    if (readRegister(REG_TIME_RAINFALL, buff, 4) == 0) {
        return -1;
    }
    uint32_t rainfall = (buff | (buff << 8) | (buff << 16) | (buff << 24));
    return rainfall / 10000.0f;
}

uint32_t DFRobotSen0575I2C::getRawData() {
    uint8_t buff = {0};
    readRegister(REG_RAW_DATA, buff, 4);
    uint32_t rawdata = (buff | (buff << 8) | (buff << 16) | (buff << 24));
    return rawdata;
}

float DFRobotSen0575I2C::getSensorWorkingTime() {
    uint8_t buff = {0};
    readRegister(REG_SYS_TIME, buff, 2);
    uint16_t workingTime = (buff | (buff << 8));
    return workingTime / 60.0f;
}

uint8_t DFRobotSen0575I2C::setRainAccumulatedValue(float accumulatedValue) {
    uint16_t data = accumulatedValue * 10000;
    uint8_t buff = {data & 0xFF, data >> 8};
    return writeRegister(REG_BASE_RAINFALL, buff, 2);
}

bool DFRobotSen0575I2C::begin() {
    uint8_t buff = {0};
    readRegister(REG_PID, buff, 4);
    uint32_t pid = (buff | (buff << 8) | (buff << 16) | (buff << 24));
    uint32_t vid = (buff | (buff & 0x3F) << 8);
    return (vid == 0x3343 && pid == 0x100C0);
}

uint8_t DFRobotSen0575I2C::readRegister(uint8_t reg, void* pBuf, size_t size) {
    this->write(this->_deviceAddr, reg);
    if (this->read(this->_deviceAddr, pBuf, size) != size) {
        ESP_LOGE(TAG, "DFRobot SEN0575 '%s' read error", this->name_.c_str());
        return 0;
    }
    return size;
}

uint8_t DFRobotSen0575I2C::writeRegister(uint8_t reg, void* pBuf, size_t size) {
    this->write(this->_deviceAddr, reg, pBuf, size);
    delay(100);
    return 0;
}

} // namespace dfrobot_sen0575_i2c
} // namespace esphome
