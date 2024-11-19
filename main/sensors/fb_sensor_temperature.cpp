#include "fb_sensor_temperature.hpp"

#include <ranges>

#include "ds18b20.h"
#include "fb_globals.hpp"



using namespace fb;
using namespace sensor;



// TemperatureSensor::TemperatureSensor(OW_t& interface)
// 	: _interface(interface)
// {

// }

const char* TemperatureSensor::getName() const
{
	return "TemperatureSensor";
}

void TemperatureSensor::lateInit(Id id, OW_t* interface)
{
	_id = id;
	_interface = interface;
}

float TemperatureSensor::getValue() const
{
	return _value;
}

TemperatureSensor::Id TemperatureSensor::getId() const
{
	return _id;
}

bool TemperatureSensor::_doInit()
{
	//TODO: check interface status

	return getId() != InvalidId;
}

SensorIface::UpdateResult TemperatureSensor::_doUpdate()
{
	_temperatureMeasureRequest();
	const float current = _temperatureValueRequest();

	if(current == TemperatureSensor::InvalidValue){
		FB_DEBUG_LOG("Temperature[0x%llX] lost detected", getId());
		return SensorIface::UpdateResult::FAIL;
	}

	if(current != _value){
		FB_DEBUG_LOG("Temperature[0x%llX] changed %.2f -> %.2f", getId(), _value, current);
		_value = current;

		return SensorIface::UpdateResult::VALUE_CHANGED;
	}

	return SensorIface::UpdateResult::OK;
}

void TemperatureSensor::_temperatureMeasureRequest()
{
	//TODO: user target command without skipping ROM
	ow_reset(_interface);
	// ow_send(&_interface, OW_SKIP_ROM);
	ow_send(_interface, OW_MATCH_ROM);
	for (int b = 0; b < 64; b += 8) {
		ow_send(_interface, getId() >> b);
	}
	ow_send(_interface, DS18B20_CONVERT_T);
}

float TemperatureSensor::_temperatureValueRequest()
{
	if (!ow_read(_interface) == false)
	{
		// portYIELD();
		return TemperatureSensor::InvalidValue;
	}
	
	ow_reset(_interface);
	ow_send(_interface, OW_MATCH_ROM);
	for (int b = 0; b < 64; b += 8) {
		ow_send(_interface, getId() >> b);
	}
	
	ow_send(_interface, DS18B20_READ_SCRATCHPAD);

	int16_t temp = 0;
	temp = ow_read(_interface) | (ow_read(_interface) << 8);

	return temp > 0 ? temp / 16.0 : TemperatureSensor::InvalidValue;
}



template<int N>
TemperatureSensorArray<N>::TemperatureSensorArray(int gpio)
{
	assert(ow_init(&_interface, gpio));
	// std::fill_n(std::back_insert_iterator(_sensors.begin()), expectedDevices, TemperatureSensor{_interface});
	// for(int i = 0; i < N; i++){
		// _sensors[i]
	// 	_sensors.push_back(TemperatureSensor{_interface});
	// }
}

template<int N>
TemperatureSensorArray<N>::~TemperatureSensorArray()
{
	ow_deinit(&_interface);
}

template<int N>
const char* TemperatureSensorArray<N>::getName() const
{
	return "TemperatureSensorArray";
}

template<int N>
int TemperatureSensorArray<N>::getDeviceCount() const
{
	return _sensors.size();
}

template<int N>
const TemperatureSensor& TemperatureSensorArray<N>::getSensor(int index) const
{
	return _sensors[index];
}

template<int N>
TemperatureSensor* TemperatureSensorArray<N>::getSensor(int index)
{
	return &_sensors[index];
}

template<int N>
bool TemperatureSensorArray<N>::_doInit()
{
	if (!ow_reset(&_interface)){
		return false;
	}

	//обнуляем состояния
	// std::for_each(_sensors.begin(), _sensors.end(), [](auto& s){s._resetState();});
	TemperatureSensor::Id sensors[10];

	const int num_devs = ow_romsearch(&_interface, sensors, sizeof(sensors) / sizeof(sensors[0]), OW_SEARCH_ROM);

	//TODO: change to ranges
	for(int i = 0; i < num_devs; i++){
		const auto id = sensors[i];
		auto iter = std::find_if(_sensors.begin(), _sensors.end(),
			[id](const TemperatureSensor& s){return s.getId() == id;});
		
		if(iter == _sensors.end()){
			//find first not initialized and initialize it
			auto notInitIter = std::find_if(_sensors.begin(), _sensors.end(),
				[id](const TemperatureSensor& s){return s.getId() == TemperatureSensor::InvalidId;});
			
			assert(notInitIter != _sensors.end());
			(*notInitIter).lateInit(id, &_interface);
		}
	}

	bool result = true;
	for(int i = 0; i < _sensors.size(); i++){
		result &= _sensors[i].init();
	}

	return result;
}

template<int N>
SensorIface::UpdateResult TemperatureSensorArray<N>::_doUpdate()
{
	bool result = true;
	for(int i = 0; i < _sensors.size(); i++){
		result &= _sensors[i].isInit();
	}

	return result ? SensorIface::UpdateResult::OK : SensorIface::UpdateResult::FAIL;
}




// TempreatureSensorTest::TempreatureSensorTest(int gpio, int expectedDevices)
// 	: _expectedDevices(expectedDevices)
// {
// 	assert(ow_init(&_interface, gpio));
// }

// TempreatureSensorTest::~TempreatureSensorTest()
// {
// 	ow_deinit(&_interface);
// }

// const char* TempreatureSensorTest::getName() const
// {
// 	return "TempreatureSensor";
// }

// float TempreatureSensorTest::getValue(int index) const
// {
// 	if(index >= _sensors.size()){
// 		return InvalidValue;
// 	}

// 	return _sensors[index].value;
// }

// int TempreatureSensorTest::getDeviceCount() const
// {
// 	return _expectedDevices;
// }

// const TemperatureSensor& TempreatureSensorTest::getSensor(int index) const
// {
// 	assert(index >= 0 && index < _sensors.size());
	
// 	return _sensors[index];
// }

// bool TempreatureSensorTest::_doInit()
// {
// 	if (!ow_reset(&_interface)){
// 		return false;
// 	}

// 	//обнуляем состояния
// 	std::for_each(_sensors.begin(), _sensors.end(), [](auto& s){s.alive = false; s.value = InvalidValue;});
// 	TemperatureSensor::Id sensors[10];

// 	const int num_devs = ow_romsearch(&_interface, sensors, sizeof(sensors) / sizeof(sensors[0]), OW_SEARCH_ROM);

// 	//TODO: change to ranges
// 	for(int i = 0; i < num_devs; i++){
// 		const auto id = sensors[i];
// 		auto iter = std::find_if(_sensors.begin(), _sensors.end(),
// 			[id](const TemperatureSensor& s){return s.id == id;});
		
// 		if(iter == _sensors.end()){
// 			_sensors.insert(_sensors.end(), {true, InvalidValue, sensors[i]});
// 		}else{
// 			(*iter).alive = true;
// 		}
// 	}

// 	return num_devs == _expectedDevices;
// }

// bool TempreatureSensorTest::_doUpdate()
// {
// 	_temperatureMeasureRequest();
// 	//TODO: change to ranges
// 	std::for_each(_sensors.begin(), _sensors.end(),
// 		[this](auto& entry){
// 			entry.value = _temperatureValueRequest(entry.id);
// 			FB_DEBUG_LOG("Temperature[0x%llX] = %.2f", entry.id, entry.value);
// 		});
	
// 	//TODO: made some checks if sensor was lost through some of operations
// 	return true;
// }

// void TempreatureSensorTest::_temperatureMeasureRequest()
// {
// 	ow_reset(&_interface);
// 	ow_send(&_interface, OW_SKIP_ROM);
// 	ow_send(&_interface, DS18B20_CONVERT_T);
// }

// float TempreatureSensorTest::_temperatureValueRequest(TemperatureSensor::Id id)
// {
// 	while (ow_read(&_interface) == false)
// 	{
// 		portYIELD();
// 	}
	
// 	ow_reset(&_interface);
// 	ow_send(&_interface, OW_MATCH_ROM);
// 	for (int b = 0; b < 64; b += 8) {
// 		ow_send(&_interface, id >> b);
// 	}
	
// 	ow_send(&_interface, DS18B20_READ_SCRATCHPAD);

// 	int16_t temp = 0;
// 	temp = ow_read(&_interface) | (ow_read(&_interface) << 8);

// 	return temp / 16.0;
// }