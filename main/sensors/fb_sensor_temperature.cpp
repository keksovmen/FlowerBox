#include "fb_sensor_temperature.hpp"

#include <ranges>

#include "ds18b20.h"
#include "fb_globals.hpp"



using namespace fb;
using namespace sensor;



TempreatureSensorTest::TempreatureSensorTest(int gpio)
{
	assert(ow_init(&_interface, gpio));
}

TempreatureSensorTest::~TempreatureSensorTest()
{
	ow_deinit(&_interface);
}

const char* TempreatureSensorTest::getName()
{
	return "TempreatureSensor";
}

float TempreatureSensorTest::getValue() const
{
	return _value;
}

bool TempreatureSensorTest::_doInit()
{
	if (!ow_reset(&_interface)){
		return false;
	}

	_sensors.clear();
	// std::vector<TemperatureSensor::Id> result;
	TemperatureSensor::Id sensors[10];

	const int num_devs = ow_romsearch(&_interface, sensors, sizeof(sensors) / sizeof(sensors[0]), OW_SEARCH_ROM);
	// result.insert(result.begin(), sensors, sensors + num_devs);

	//TODO: change to ranges
	for(int i = 0; i < num_devs; i++){
		_sensors.insert(_sensors.end(), {true, -255, sensors[i]});
	}

	//TODO: update current sensor states, change alive flag accordingly


	return num_devs > 0;
}

bool TempreatureSensorTest::_doUpdate()
{
	_temperatureMesureRequest();
	//TODO: change to ranges
	std::for_each(_sensors.begin(), _sensors.end(),
		[this](auto& entry){
			entry.value = _tempreatureValueRequest(entry.id);
			this->_value = entry.value; 
			FB_DEBUG_LOG("Temperature = %.2f", this->_value);
		});
	
	//TODO: made some checks if sensor was lost through some of operations
	return true;
}

void TempreatureSensorTest::_temperatureMesureRequest()
{
	ow_reset(&_interface);
	ow_send(&_interface, OW_SKIP_ROM);
	ow_send(&_interface, DS18B20_CONVERT_T);
}

float TempreatureSensorTest::_tempreatureValueRequest(TemperatureSensor::Id id)
{
	while (ow_read(&_interface) == false)
	{
		portYIELD();
	}
	
	ow_reset(&_interface);
	ow_send(&_interface, OW_MATCH_ROM);
	for (int b = 0; b < 64; b += 8) {
		ow_send(&_interface, id >> b);
	}
	
	ow_send(&_interface, DS18B20_READ_SCRATCHPAD);

	int16_t temp = 0;
	temp = ow_read(&_interface) | (ow_read(&_interface) << 8);

	return temp / 16.0;
}