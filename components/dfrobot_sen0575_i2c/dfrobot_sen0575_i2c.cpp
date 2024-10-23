#include "dfrobot_sen0575_i2c.h"
#include "esphome/core/helpers.h"
#include "esphome/core/log.h"

namespace esphome {
namespace dfrobot_sen0575_i2c {

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
    ESP_LOGCONFIG(TAG, "Setting up DFRobot SEN0575 ...");
    if (!begin_()) {
        ESP_LOGE("DFRobotSen0575I2C", "Failed to initialize sensor");
        this->mark_failed();
        return;
    }
    ESP_LOGCONFIG(TAG, "DFRobot SEN0575 (Firmware: %s) setup complete.", this->getFirmwareVersion_().c_str());
}

void DFRobotSen0575I2C::loop() {}

void DFRobotSen0575I2C::update() {
    if (this->cumulative_rainfall_ != nullptr)
      this->cumulative_rainfall_->publish_state(this->getRainfall_());
    if (this->rainfall_within_hour_ != nullptr)
      this->rainfall_within_hour_->publish_state(this->getRainfall_(1));
    if (this->raw_data_ != nullptr)
      this->raw_data_->publish_state(this->getRawData_());
    if (this->working_time_ != nullptr)
      this->working_time_->publish_state(this->getWorkingTime_());
}

void DFRobotSen0575I2C::dump_config() {
    ESP_LOGCONFIG(TAG, "DFRobot Sen0575 I2C:");
    LOG_I2C_DEVICE(this);
    ESP_LOGCONFIG(TAG, "Firmware: %s", this->getFirmwareVersion_().c_str());
    ESP_LOGCONFIG(TAG, "Cumulative Rainfall: %s", this->getRainfall_().c_str());
    ESP_LOGCONFIG(TAG, "Rainfall Within Hour: %s", this->getRainfall_(1).c_str());
    ESP_LOGCONFIG(TAG, "Raw Data: %s", this->getRawData_().c_str());
    ESP_LOGCONFIG(TAG, "Working Time: %s", this->getWorkingTime_().c_str());
    if (this->is_failed()) {
        ESP_LOGE(TAG, "Communication with DFRobot SEN0575 failed!");
    }
    LOG_UPDATE_INTERVAL(this);
    LOG_SENSOR("  ", "Cumulative Rainfall", this->cumulative_rainfall_);
    LOG_SENSOR("  ", "Rainfall Within Hour", this->rainfall_within_hour_);
    LOG_SENSOR("  ", "Raw Data", this->raw_data_);
    LOG_SENSOR("  ", "Working Time", this->working_time_);
}

std::string DFRobotSen0575I2C::getFirmwareVersion_() {
    uint16_t version = 0;
    uint8_t buff[2] = {0};
    readRegister_(REG_VERSION, buff, 2);
    version = buff[0] | ( ((uint16_t)buff[1]) << 8 );
    std::string version_str = std::to_string(version >> 12) + '.' + std::to_string((version >> 8) & 0x0F) + '.' + std::to_string((version >> 4) & 0x0F) + '.' + std::to_string(version & 0x0F);
    ESP_LOGD(TAG, "DFRobot SEN0575 firmware version: %d", version_str);
    return version_str;
}
float DFRobotSen0575I2C::getRainfall_() {
    uint8_t buff[4] = {0};
    readRegister_(REG_CUMULATIVE_RAINFALL, buff, 4);
    uint32_t rainfall = (buff[0] | (buff[1] << 8) | (buff[2] << 16) | (buff[3] << 24));
    ESP_LOGD(TAG, "DFRobot SEN0575 rainfall: %d", rainfall / 10000.0f);
    return rainfall / 10000.0f;
}

float DFRobotSen0575I2C::getRainfall_(uint8_t hour) {
    writeRegister_(REG_RAIN_HOUR, &hour, 1);
    uint8_t buff[4] = {0};
    if (readRegister_(REG_TIME_RAINFALL, buff, 4) == 0) {
        return -1;
    }
    uint32_t rainfall = (buff[0] | (buff[1] << 8) | (buff[2] << 16) | (buff[3] << 24));
    ESP_LOGD(TAG, "DFRobot SEN0575 rainfall (%d h): %d", hour, rainfall / 10000.0f);
    return rainfall / 10000.0f;
}

uint32_t DFRobotSen0575I2C::getRawData_() {
    uint8_t buff[4] = {0};
    readRegister_(REG_RAW_DATA, buff, 4);
    uint32_t rawdata = (buff[0] | (buff[1] << 8) | (buff[2] << 16) | (buff[3] << 24));
    ESP_LOGD(TAG, "DFRobot SEN0575 raw data: %d", rawdata);
    return rawdata;
}

float DFRobotSen0575I2C::getWorkingTime_() {
    uint8_t buff[2] = {0};
    readRegister_(REG_SYS_TIME, buff, 2);
    uint16_t workingTime = (buff[0] | (buff[1] << 8));
    ESP_LOGD(TAG, "DFRobot SEN0575 working time: %d", workingTime / 60.0f);
    return workingTime / 60.0f;
}

uint8_t DFRobotSen0575I2C::setRainAccumulatedValue_(float accumulatedValue) {
    uint16_t data = accumulatedValue * 10000;
    uint8_t buff[2] = {static_cast<uint8_t>(data & 0xFF), static_cast<uint8_t>(data >> 8)};
    return writeRegister_(REG_BASE_RAINFALL, buff, 2);
}

bool DFRobotSen0575I2C::begin_() {

    uint8_t buff[4] = {0};
    this->readRegister_(REG_PID, buff, 4);
    uint32_t pid = (buff[0] | (buff[1] << 8) | (buff[2] << 16) | (buff[3] << 24));
    uint32_t vid = (buff[0] | (buff[1] & 0x3F) << 8);
    ESP_LOGD(TAG, "DFRobot SEN0575 begin: vid:%d, pid:%d", vid, pid);
    return (vid == 0x3343 && pid == 0x100C0);
}

uint8_t DFRobotSen0575I2C::readRegister_(uint8_t reg, uint8_t *pBuf, size_t size) {
    if (this->read_register(reg, pBuf, size) != i2c::ERROR_OK)
    {
        ESP_LOGE(TAG, "DFRobot SEN0575 read error register: %d, %d, %d", reg, pBuf, size);
        return 0;
    }
    ESP_LOGD(TAG, "DFRobot SEN0575 read register: %d, %d, %d", reg, pBuf, size);
    return size;
}

uint8_t DFRobotSen0575I2C::writeRegister_(uint8_t reg, uint8_t *pBuf, size_t size) {
    this->write_register(reg, pBuf, size);
    ESP_LOGD(TAG, "DFRobot SEN0575 write register: %d, %d, %d", reg, pBuf, size);
    delay(100);
    return 0;
}

} // namespace dfrobot_sen0575_i2c
} // namespace esphome
