from esphome.components import i2c, sensor
from esphome.const import CONF_ID, CONF_NAME

DEPENDENCIES = ['i2c']

def setup_sensor(hass, config, i2c, id):
    rainfall_sensor = DFRobotRainfallSensor(i2c, id, config)
    yield rainfall_sensor

class DFRobotRainfallSensor(sensor.Sensor, i2c.I2CDevice):
    def __init__(self, i2c, id, config):
        super().__init__()
        self._i2c = i2c
        self._id = id
        self._name = config[CONF_NAME]
        self._device_addr = 0x1D

    def setup(self):
        self._i2c.begin()
        if not self.begin():
            ESP_LOGE("DFRobotRainfallSensor", "Failed to initialize sensor")

    def update(self):
        # Read cumulative rainfall
        buff = self.read_register(self._device_addr, I2C_REG_CUMULATIVE_RAINFALL, 4)
        rainfall = (buff | (buff << 8) | (buff << 16) | (buff << 24)) / 10000.0
        self.publish_state(rainfall)

    def read_register(self, addr, reg, size):
        return self._i2c.read(addr, reg, size)

    def write_register(self, addr, reg, data, size):
        return self._i2c.write(addr, reg, data, size)

    @property
    def name(self):
        return self._name

    @property
    def unique_id(self):
        return self._id

    @property
    def unit_of_measurement(self):
        return 'mm'

    @property
    def device_class(self):
        return 'rainfall'