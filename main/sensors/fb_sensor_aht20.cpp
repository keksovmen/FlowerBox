#include "fb_sensor_aht20.hpp"

#include "driver/i2c_master.h"



using namespace fb;
using namespace sensor;



SensorAht20::SensorAht20(i2c_port_t port, int gpioSda, int gpioScl)
{
	i2c_master_bus_config_t i2c_conf = {
		.i2c_port = port,
		.sda_io_num = (gpio_num_t) gpioSda,
		.scl_io_num = (gpio_num_t) gpioScl,
		.clk_source = I2C_CLK_SRC_DEFAULT,
		.glitch_ignore_cnt = 0,
		.intr_priority = 0,
		.trans_queue_depth = 0,
		.flags = {.enable_internal_pullup = 1},
	};

	ESP_ERROR_CHECK(i2c_new_master_bus(&i2c_conf, &_bus));

	aht20_i2c_config_t cfg = {
		.bus = _bus,
	};
	ESP_ERROR_CHECK(aht20_new_sensor(&cfg, &_hndl));
}

SensorAht20::~SensorAht20()
{
	ESP_ERROR_CHECK(i2c_del_master_bus(_bus));
}

const char* SensorAht20::getName() const
{
	return "SensorAht20";
}

float SensorAht20::getValue() const
{
	return getTemperature();
}

float SensorAht20::getValueIndexed(int index) const
{
	return index == ValueTemperatureIndex ? getTemperature() : getHumidity();
}

int SensorAht20::getIndexCount() const
{
	return 2;
}

float SensorAht20::getTemperature() const
{
	return _temperature;
}

float SensorAht20::getHumidity() const
{
	return _humidity;
}

bool SensorAht20::_doInit()
{
	return i2c_master_probe(_bus, AHT20_ADDRRES, 500) == ESP_OK;
}

SensorIface::UpdateResult SensorAht20::_doUpdate()
{
	float temp = 0;
	float hum = 0;
	const esp_err_t err = aht20_read_temperature_humidity(_hndl, &temp, &hum);

	if(err == ESP_FAIL){
		FB_DEBUG_LOG_I_OBJ("failed to update");
		return SensorIface::UpdateResult::FAIL;
	}

	auto result = SensorIface::UpdateResult::OK;

	if(temp != _temperature || hum == _humidity){
		FB_DEBUG_LOG_I_OBJ("Value changed temp: %.2f -> %.2f, hum: %.2f -> %.2f", _temperature, temp, _humidity, hum);

		_temperature = temp;
		_humidity = hum;
		result = SensorIface::UpdateResult::VALUE_CHANGED;
	}

	return result;
}
