# DFRobot Rainfall Sensor (SEN0575) ESPHome Component

## Overview

This ESPHome component integrates the DFRobot Rainfall Sensor (SEN0575) into your home automation system. The sensor provides cumulative rainfall data, rainfall within a specified time, raw data, and sensor working time.

## Installation

To use this component, follow these steps:

### Step 1: Add the Component to Your ESPHome Configuration

Add the following lines to your `configuration.yaml` file to include the custom component:

```yaml
external_components:
  - source: github://foreign-sub/DFRobot-Rainfall-Sensor-SEN0575-
    components: [dfrobot_rainfall_sensor]
```

Alternatively, you can download the component and place it in your custom_components directory.

### Step 2: Configure the I2C Interface

Ensure your ESPHome device is configured to use the I2C interface. Here is an example configuration:

```yaml
i2c:
  sda: GPIO21
  scl: GPIO22
```

Step 3: Add the Sensor to Your Configuration
Add the sensor to your configuration.yaml file:

```yaml
sensor:
  - platform: custom
    lambda: |-
      auto rainfall_sensor = new DFRobotRainfallSensor(i2c::I2CDevice(*this), 0x1D);
      App.register_component(rainfall_sensor);
      return {rainfall_sensor};
    sensors:
      - name: "Rainfall Sensor"
        unit_of_measurement: "mm"
        device_class: "rainfall"
        accuracy_decimals: 2
        id: rainfall_sensor_id

      - name: "Rainfall within 1 hour"
        unit_of_measurement: "mm"
        device_class: "rainfall"
        accuracy_decimals: 2
        lambda: |-
          auto sensor = id(rainfall_sensor_id);
          return sensor->getRainfall(1);
        update_interval: 60s

      - name: "Raw Data"
        unit_of_measurement: "count"
        device_class: "none"
        accuracy_decimals: 0
        lambda: |-
          auto sensor = id(rainfall_sensor_id);
          return sensor->getRawData();
        update_interval: 60s

      - name: "Sensor Working Time"
        unit_of_measurement: "h"
        device_class: "duration"
        accuracy_decimals: 2
        lambda: |-
          auto sensor = id(rainfall_sensor_id);
          return sensor->getSensorWorkingTime();
        update_interval: 60s
```

or use the example yaml files in the directory.

## Usage

### Cumulative Rainfall

The Rainfall Sensor entity will display the cumulative rainfall since the sensor started working.

### Rainfall within a Specified Time

The Rainfall within 1 hour entity will display the cumulative rainfall within the last hour. You can adjust the time period by changing the hour parameter in the lambda function.
Raw Data
The Raw Data entity will display the number of tipping bucket counts, which can be used to calculate rainfall.

### Sensor Working Time

The Sensor Working Time entity will display the total working time of the sensor in hours.

## Methods

begin()
Initializes the sensor and checks for successful communication.

getFirmwareVersion()
Returns the firmware version of the sensor.

getRainfall()
Returns the cumulative rainfall since the sensor started working.

getRainfall(uint8_t hour)
Returns the cumulative rainfall within the specified time (1-24 hours).

getRawData()
Returns the number of tipping bucket counts.

setRainAccumulatedValue(float accumulatedValue)
Sets the accumulated rainfall value.

getSensorWorkingTime()
Returns the working time of the sensor in hours.

## Troubleshooting

Ensure the I2C pins are correctly configured.
Verify that the sensor is properly connected to the I2C bus.
Check the sensor's address and ensure it matches the configuration.

## Credits

This component is based on the DFRobot Rainfall Sensor library and adapted for ESPHome.

## License

This component is released under the MIT License.

## Contributing

Contributions are welcome. Please submit pull requests or issues on the GitHub repository.

## Compatibility

This component has been tested with the following microcontrollers:
ESP32
ESP8266
Please report any compatibility issues or additional supported microcontrollers.