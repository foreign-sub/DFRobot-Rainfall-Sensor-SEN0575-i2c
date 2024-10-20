# DFRobot Rainfall Sensor (SEN0575) ESPHome Component

## Overview

This ESPHome component integrates the DFRobot Rainfall Sensor (SEN0575) into your home automation system. The sensor provides cumulative rainfall data, rainfall within a specified time, raw data, and sensor working time.

## Installation

To use this component, follow these steps:

### Step 1: Add the Component to Your ESPHome Configuration

Add the following lines to your `configuration.yaml` file to include the custom component:

```yaml
external_components:
  - source: github://foreign-sub/DFRobot-Rainfall-Sensor-SEN0575-i2c
```

Alternatively, you can download the component and place it in your custom_components directory.

### Step 2: Configure the I2C Interface

Ensure your ESPHome device is configured to use the I2C interface. Here is an example configuration:

```yaml
i2c:
  sda: GPIO21
  scl: GPIO22
  frequency: 100000.0
  id: bus_a
```

Step 3: Add the Sensor to Your Configuration
Add the sensor to your configuration.yaml file:

```yaml
sensor:
  - platform: dfrobot_sen0575_i2c
    i2c_id: bus_a
    #id: rainfall_sensor # optional id
    cumulative_rainfall:
      name: "Cumulative Rainfall"
    rainfall_within_hour:
      name: "Rainfall within hour"
    raw_data:
      name: "Raw Data"
    working_time:
      name: "Working Time"
```

or use the example yaml files.

## Usage

### Cumulative Rainfall

The Rainfall Sensor entity will display the cumulative rainfall since the sensor started working.

### Rainfall within a Specified Time

The Rainfall within 1 hour entity will display the cumulative rainfall within the last hour.

### Raw Data

The Raw Data entity will display the number of tipping bucket counts, which can be used to calculate rainfall.

### Sensor Working Time

The Sensor Working Time entity will display the total working time of the sensor in hours.

## Methods

```cpp
begin()
```

Initializes the sensor and checks for successful communication.

```cpp
getFirmwareVersion()
```

Returns the firmware version of the sensor.

```cpp
getRainfall()
```

Returns the cumulative rainfall since the sensor started working.

```cpp
getRainfall(uint8_t hour)
```

Returns the cumulative rainfall within the specified time (1-24 hours).

```cpp
getRawData()
```

Returns the number of tipping bucket counts.

```cpp
setRainAccumulatedValue(float accumulatedValue)
```

Sets the accumulated rainfall value.

```cpp
getWorkingTime()
```

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

This component has not been tested with the following microcontrollers:

ESP32

ESP8266

Please report any compatibility issues or additional supported microcontrollers.
