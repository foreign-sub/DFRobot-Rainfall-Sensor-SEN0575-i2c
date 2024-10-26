#include "dfrobot_sen0575_i2c.h"
#include "esphome/core/helpers.h"
#include "esphome/core/log.h"

namespace esphome {
namespace dfrobot_sen0575_i2c {

static const uint8_t REGISTER_PRODUCT_ID = 0x00;
static const uint8_t REGISTER_VENDOR_ID = 0x02;
static const uint8_t REGISTER_VERSION = 0x0A;
static const uint8_t REGISTER_TIME_RAINFALL = 0x0C;
static const uint8_t REGISTER_CUMULATIVE_RAINFALL = 0x10;
static const uint8_t REGISTER_RAW_DATA = 0x14;
static const uint8_t REGISTER_SYSTEM_TIME = 0x18;
static const uint8_t REGISTER_RAIN_HOUR = 0x26;
static const uint8_t REGISTER_BASE_RAINFALL = 0x28;

static const char *const TAG = "dfrobot_sen0575_i2c.sensor";

void DFRobotSen0575I2C::setup() {
  ESP_LOGCONFIG(TAG, "Setting up DFRobot SEN0575 ...");
  if (!this->initialize_sensor_()) {
    ESP_LOGE(TAG, "Failed to initialize sensor");
    this->mark_failed();
    return;
  }
  ESP_LOGCONFIG(TAG, "Setup complete. (Firmware: %s)", this->get_firmware_version_().c_str());
}

void DFRobotSen0575I2C::loop() {}

void DFRobotSen0575I2C::update() {
  if (this->cumulative_rainfall_ != nullptr) {
    float rainfall = this->get_rainfall_();
    this->cumulative_rainfall_->publish_state(rainfall);
  }
  if (this->rainfall_within_hour_ != nullptr) {
    float rainfall_hour = this->get_rainfall_for_period_(this->rainfall_hour_);
    this->rainfall_within_hour_->publish_state(rainfall_hour);
  }
  if (this->raw_data_ != nullptr) {
    uint32_t raw_data = this->get_raw_data_();
    this->raw_data_->publish_state(raw_data);
  }
  if (this->working_time_ != nullptr) {
    float working_time = this->get_working_time_();
    this->working_time_->publish_state(working_time);
  }
}

void DFRobotSen0575I2C::dump_config() {
  ESP_LOGCONFIG(TAG, "DFRobot Sen0575 I2C:");
  LOG_I2C_DEVICE(this);
  LOG_UPDATE_INTERVAL(this);

  // uncomment the following lines to show the state of the sensor at config for debbugging purposes
  // ESP_LOGCONFIG(TAG, "Init: %s", this->initialize_sensor_()? "OK" : "Failed");
  // ESP_LOGCONFIG(TAG, "Firmware: %s", this->get_firmware_version_().c_str());
  // ESP_LOGCONFIG(TAG, "Cumulative Rainfall: %.2f", this->get_rainfall_());
  // ESP_LOGCONFIG(TAG, "Rainfall Within Hour: %.2f", this->get_rainfall_for_period_(1));
  // ESP_LOGCONFIG(TAG, "Raw Data: %d", this->get_raw_data_());
  // ESP_LOGCONFIG(TAG, "Working Time: %.2f", this->get_working_time_());

  if (this->is_failed()) {
    ESP_LOGE(TAG, "Communication with DFRobot SEN0575 failed!");
    if ((this->component_state_ & COMPONENT_STATE_MASK) == COMPONENT_STATE_FAILED) {
      this->component_state_ &= ~COMPONENT_STATE_MASK;
      this->component_state_ |= COMPONENT_STATE_CONSTRUCTION;
      ESP_LOGD(TAG, "Resetting state to construction");
    }
  }

  ESP_LOGCONFIG(TAG, "Rainfall Hour: %d", this->rainfall_hour_);

  LOG_SENSOR("  ", "Cumulative Rainfall", this->cumulative_rainfall_);
  LOG_SENSOR("  ", "Rainfall Within Hour", this->rainfall_within_hour_);
  LOG_SENSOR("  ", "Raw Data", this->raw_data_);
  LOG_SENSOR("  ", "Working Time", this->working_time_);
}

std::string DFRobotSen0575I2C::get_firmware_version_() {
  uint16_t version = 0;
  uint8_t buffer[2] = {0};
  this->read_register_(REGISTER_VERSION, buffer, 2);
  version = buffer[0] | (static_cast<uint16_t>(buffer[1]) << 8);

  std::string version_str = std::to_string(version >> 12) + '.' +
                            std::to_string((version >> 8) & 0x0F) + '.' +
                            std::to_string((version >> 4) & 0x0F) + '.' +
                            std::to_string(version & 0x0F);
  ESP_LOGD(TAG, "DFRobot SEN0575 firmware version: %s", version_str.c_str());
  return version_str;
}

float DFRobotSen0575I2C::get_rainfall_() {
  uint8_t buffer[4] = {0};
  this->read_register_(REGISTER_CUMULATIVE_RAINFALL, buffer, 4);
  uint32_t rainfall = (buffer[0] | (buffer[1] << 8) | (buffer[2] << 16) | (buffer[3] << 24));
  ESP_LOGD(TAG, "DFRobot SEN0575 rainfall: %.2f", rainfall / 10000.0f);
  return rainfall / 10000.0f;
}

float DFRobotSen0575I2C::get_rainfall_for_period_(uint8_t hours) {
  this->write_register_(REGISTER_RAIN_HOUR, &hours, 1);
  uint8_t buffer[4] = {0};
  if (this->read_register_(REGISTER_TIME_RAINFALL, buffer, 4) == 0) {
    return -1;
  }
  uint32_t rainfall = (buffer[0] | (buffer[1] << 8) | (buffer[2] << 16) | (buffer[3] << 24));
  ESP_LOGD(TAG, "DFRobot SEN0575 rainfall (%d h): %.2f", hours, rainfall / 10000.0f);
  return rainfall / 10000.0f;
}

uint32_t DFRobotSen0575I2C::get_raw_data_() {
  uint8_t buffer[4] = {0};
  this->read_register_(REGISTER_RAW_DATA, buffer, 4);
  uint32_t raw_data = (buffer[0] | (buffer[1] << 8) | (buffer[2] << 16) | (buffer[3] << 24));
  ESP_LOGD(TAG, "DFRobot SEN0575 raw data: %d", raw_data);
  return raw_data;
}

float DFRobotSen0575I2C::get_working_time_() {
  uint8_t buffer[2] = {0};
  this->read_register_(REGISTER_SYSTEM_TIME, buffer, 2);
  uint16_t working_time = (buffer[0] | (buffer[1] << 8));
  ESP_LOGD(TAG, "DFRobot SEN0575 working time: %.2f", working_time / 60.0f);
  return working_time / 60.0f;
}

uint8_t DFRobotSen0575I2C::set_rain_accumulated_value_(float accumulated_value) {
  uint16_t data = accumulated_value * 10000;
  uint8_t buffer[2] = {static_cast<uint8_t>(data & 0xFF), static_cast<uint8_t>(data >> 8)};
  return this->write_register_(REGISTER_BASE_RAINFALL, buffer, 2);
}

bool DFRobotSen0575I2C::initialize_sensor_() {
  uint8_t buffer[4] = {0};
  this->read_register_(REGISTER_PRODUCT_ID, buffer, 4);
  uint32_t product_id = buffer[0] | (buffer[1] << 8) | ((buffer[3] & 0xC0) << 10);
  uint32_t vendor_id = buffer[2] | ((buffer[3] & 0x3F) << 8);
  ESP_LOGD(TAG, "DFRobot SEN0575 begin: vendor_id:%04x, product_id:%04x", vendor_id, product_id);
  return (vendor_id == 0x3343 && product_id == 0x100C0);
}

uint8_t DFRobotSen0575I2C::read_register_(uint8_t reg, uint8_t *buffer, size_t size) {
  if (this->read_register(reg, buffer, size) != i2c::ERROR_OK) {
    ESP_LOGE(TAG, "DFRobot SEN0575 read error register: %d, %d, %d", reg, buffer, size);
    return 0;
  }
  ESP_LOGD(TAG, "DFRobot SEN0575 read register: %d, %d, %d", reg, buffer, size);
  return size;
}

uint8_t DFRobotSen0575I2C::write_register_(uint8_t reg, uint8_t *buffer, size_t size) {
  if (this->write_register(reg, buffer, size) != i2c::ERROR_OK) {
    ESP_LOGE(TAG, "DFRobot SEN0575 write error register: %d, %d, %d", reg, buffer, size);
    return 0;
  }
  ESP_LOGD(TAG, "DFRobot SEN0575 write register: %d, %d, %d", reg, buffer, size);
  delay(12);
  return 0;
}

}  // namespace dfrobot_sen0575_i2c
}  // namespace esphome