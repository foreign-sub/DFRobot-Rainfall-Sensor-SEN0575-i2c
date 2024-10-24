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

  protected:
    void update_sensor_readings_();
    std::string get_firmware_version_();
    float get_rainfall_();
    float get_rainfall_for_period_(uint8_t hours);
    uint32_t get_raw_data_();
    float get_working_time_();
    uint8_t set_rain_accumulated_value_(float accumulated_value);
    bool initialize_sensor_();
    uint8_t read_register_(uint8_t reg, uint8_t *buffer, size_t size);
    uint8_t write_register_(uint8_t reg, uint8_t *buffer, size_t size);

    sensor::Sensor *cumulative_rainfall_{nullptr};
    sensor::Sensor *rainfall_within_hour_{nullptr};
    sensor::Sensor *raw_data_{nullptr};
    sensor::Sensor *working_time_{nullptr};
};

}  // namespace dfrobot_sen0575_i2c
}  // namespace esphome
