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
    if (this->cumulative_rainfall_ != nullptr) {
        float rainfall = this->getRainfall_();
        this->cumulative_rainfall_->publish_state(rainfall);
    }
    if (this->rainfall_within_hour_ != nullptr) {
        float rainfall_hour = this->getRainfall_(1);
        this->rainfall_within_hour_->publish_state(rainfall_hour);
    }
    if (this->raw_data_ != nullptr) {
        uint32_t raw_data = this->getRawData_();
        this->raw_data_->publish_state(raw_data);
    }
    if (this->working_time_ != nullptr) {
        float working_time = this->getWorkingTime_();
        this->working_time_->publish_state(working_time);
    }
}

void DFRobotSen0575I2C::dump_config() {
    ESP_LOGCONFIG(TAG, "DFRobot Sen0575 I2C:");
    LOG_I2C_DEVICE(this);
    ESP_LOGCONFIG(TAG, "Firmware: %s", this->getFirmwareVersion_().c_str());
    ESP_LOGCONFIG(TAG, "Cumulative Rainfall: %.2f", this->getRainfall_());
    ESP_LOGCONFIG(TAG, "Rainfall Within Hour: %.2f", this->getRainfall_(1));
    ESP_LOGCONFIG(TAG, "Raw Data: %d", this->getRawData_());
    ESP_LOGCONFIG(TAG, "Working Time: %.2f", this->getWorkingTime_());
    if (this->is_failed()) {
        ESP_LOGE(TAG, "Communication with DFRobot SEN0575 failed!");
    }
    LOG_UPDATE_INTERVAL(this);
    LOG_SENSOR("  ", "Cumulative Rainfall", this->cumulative_rainfall_);
    LOG_SENSOR("  ", "Rainfall Within Hour", this->rainfall_within_hour_);
    LOG_SENSOR("  ", "Raw Data", this->raw_data_);
    LOG_SENSOR("  ", "Working Time", this->working_time_);
}

float DFRobotSen0575I2C::get_setup_priority() const { return setup_priority::DATA; }

std::string DFRobotSen0575I2C::getFirmwareVersion_() {
    uint16_t version = 0;
    uint8_t buff[2] = {0};
    readRegister_(REG_VERSION, buff, 2);
    version = buff[0] | ( ((uint16_t)buff[1]) << 8 );
    std::string version_str = std::to_string(version >> 12) + '.' + std::to_string((version >> 8) & 0x0F) + '.' + std::to_string((version >> 4) & 0x0F) + '.' + std::to_string(version & 0x0F);
    ESP_LOGD(TAG, "DFRobot SEN0575 firmware version: %s", version_str.c_str());
    return version_str;
}
float DFRobotSen0575I2C::getRainfall_() {
    uint8_t buff[4] = {0};
    readRegister_(REG_CUMULATIVE_RAINFALL, buff, 4);
    uint32_t rainfall = (buff[0] | (buff[1] << 8) | (buff[2] << 16) | (buff[3] << 24));
    ESP_LOGD(TAG, "DFRobot SEN0575 rainfall: %.2f", rainfall / 10000.0f);
    return rainfall / 10000.0f;
}

float DFRobotSen0575I2C::getRainfall_(uint8_t hour) {
    writeRegister_(REG_RAIN_HOUR, &hour, 1);
    uint8_t buff[4] = {0};
    if (readRegister_(REG_TIME_RAINFALL, buff, 4) == 0) {
        return -1;
    }
    uint32_t rainfall = (buff[0] | (buff[1] << 8) | (buff[2] << 16) | (buff[3] << 24));
    ESP_LOGD(TAG, "DFRobot SEN0575 rainfall (%d h): %.2f", hour, rainfall / 10000.0f);
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
    ESP_LOGD(TAG, "DFRobot SEN0575 working time: %.2f", workingTime / 60.0f);
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
    if(this->write_register(reg, pBuf, size) != i2c::ERROR_OK)
    {
        ESP_LOGE(TAG, "DFRobot SEN0575 write error register: %d, %d, %d", reg, pBuf, size);
        return 0;
    }
    ESP_LOGD(TAG, "DFRobot SEN0575 write register: %d, %d, %d", reg, pBuf, size);
    delay(100);
    return 0;
}

} // namespace dfrobot_sen0575_i2c
} // namespace esphome
