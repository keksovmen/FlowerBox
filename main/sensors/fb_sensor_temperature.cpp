#include "fb_sensor_temperature.hpp"

#include <ranges>

#include "ds18b20.h"
#include "fb_globals.hpp"



using namespace fb;
using namespace sensor;



TempreatureSensorTest::TempreatureSensorTest(int gpio, int expectedDevices)
	: _expectedDevices(expectedDevices)
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

float TempreatureSensorTest::getValue(int index) const
{
	if(index >= _sensors.size()){
		return InvalidValue;
	}

	return _sensors[index].value;
}

int TempreatureSensorTest::getDeviceCount() const
{
	return _expectedDevices;
}

const TemperatureSensor& TempreatureSensorTest::getSensor(int index) const
{
	assert(index >= 0 && index < _sensors.size());
	
	return _sensors[index];
}

bool TempreatureSensorTest::_doInit()
{
	if (!ow_reset(&_interface)){
		return false;
	}

	//обнуляем состояния
	std::for_each(_sensors.begin(), _sensors.end(), [](auto& s){s.alive = false; s.value = InvalidValue;});
	TemperatureSensor::Id sensors[10];

	const int num_devs = ow_romsearch(&_interface, sensors, sizeof(sensors) / sizeof(sensors[0]), OW_SEARCH_ROM);

	//TODO: change to ranges
	for(int i = 0; i < num_devs; i++){
		const auto id = sensors[i];
		auto iter = std::find_if(_sensors.begin(), _sensors.end(),
			[id](const TemperatureSensor& s){return s.id == id;});
		
		if(iter == _sensors.end()){
			_sensors.insert(_sensors.end(), {true, InvalidValue, sensors[i]});
		}else{
			(*iter).alive = true;
		}
	}

	return num_devs == _expectedDevices;
}

bool TempreatureSensorTest::_doUpdate()
{
	_temperatureMesureRequest();
	//TODO: change to ranges
	std::for_each(_sensors.begin(), _sensors.end(),
		[this](auto& entry){
			entry.value = _tempreatureValueRequest(entry.id);
			FB_DEBUG_LOG("Temperature[0x%llX] = %.2f", entry.id, entry.value);
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