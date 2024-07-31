#include "fb_sensor_service.hpp"

#include "ds18b20.h"



using namespace fb;
using namespace sensor;



static const char* TAG = "SensorService";



SensorService::SensorService(int gpio)
	: _gpio(gpio)
{
}

const char *SensorService::getName()
{
	return "SensorService";
}

void SensorService::start()
{
	assert(ow_init(&_wireIface, _gpio));

	_mutex = xSemaphoreCreateMutex();
	assert(_mutex);

	_actionQueue = xQueueCreate(5, sizeof(SensorService::Action));
	assert(_actionQueue);

	auto err = xTaskCreate(&SensorService::_task, getName(), 2 * 1024, this, 10, &_taskHndl);
	assert(err == pdPASS);
}

void SensorService::stop()
{
}

void SensorService::forseScan()
{
	assert(_actionQueue);

	auto action = SensorService::Action(&SensorService::_scanAction);
	auto err = xQueueSend(_actionQueue, &action, portMAX_DELAY);

	assert(err == pdPASS);
}

void SensorService::forseRead()
{
	assert(_actionQueue);

	auto action = SensorService::Action(&SensorService::_temperatureAction);
	auto err = xQueueSend(_actionQueue, &action, portMAX_DELAY);
	
	assert(err == pdPASS);
}

std::vector<TemperatureSensor> SensorService::getSensors() const
{
	// TODO: use range library to convert unordered map to list and then to values only
	return {};
}

void SensorService::_task(void *arg)
{
	SensorService *me = reinterpret_cast<SensorService *>(arg);

	for (;;)
	{
		SensorService::Action action;

		auto err = xQueueReceive(me->_actionQueue, &action, portMAX_DELAY);
		assert(err == pdPASS);

		action(me);
	}

	vTaskDelete(NULL);
}

std::vector<TemperatureSensor::Id> SensorService::_scanRequest()
{
	if (!ow_reset(&_wireIface)){
		return {};
	}

	std::vector<TemperatureSensor::Id> result;
	TemperatureSensor::Id sensors[10];

	const int num_devs = ow_romsearch(&_wireIface, sensors, sizeof(sensors) / sizeof(sensors[0]), OW_SEARCH_ROM);
	result.insert(result.begin(), sensors, sensors + num_devs);

	return result;
}

void SensorService::_temperatureMesureRequest()
{
	ow_reset(&_wireIface);
	ow_send(&_wireIface, OW_SKIP_ROM);
	ow_send(&_wireIface, DS18B20_CONVERT_T);
}

float SensorService::_tempreatureValueRequest(TemperatureSensor::Id id)
{
	while (ow_read(&_wireIface) == false)
	{
		portYIELD();
	}
	
	ow_reset(&_wireIface);
	ow_send(&_wireIface, OW_MATCH_ROM);
	for (int b = 0; b < 64; b += 8) {
		ow_send(&_wireIface, id >> b);
	}
	
	ow_send(&_wireIface, DS18B20_READ_SCRATCHPAD);

	int16_t temp = 0;
	temp = ow_read(&_wireIface) | (ow_read(&_wireIface) << 8);

	return temp / 16.0;
}

void SensorService::_updateSensorStates(const std::vector<TemperatureSensor::Id> &alive)
{
	for (auto id : alive)
	{
		if (_tempSensorList.contains(id))
		{
			auto &sensor = _tempSensorList[id];
			if (!sensor.alive)
			{
				sensor.alive = true;
				// TODO: drop state changed event
			}
		}
		else
		{
			_tempSensorList[id] = TemperatureSensor{true, 0.0f, id};
			// TODO: drop new sensor event
			// TODO: drop state changed event for that sensor
		}
	}
}

void SensorService::_updateTemperatureValue(TemperatureSensor::Id id, float value)
{
	assert(_tempSensorList.contains(id));

	auto &sensor = _tempSensorList[id];
	if (sensor.value != value)
	{
		sensor.value = value;
		// TODO: drop temperature change event
	}
}

void SensorService::_scanAction(SensorService* me)
{
	const auto list = me->_scanRequest();

	FB_DEBUG_TAG_LOG("Scan action find %d devices", list.size());

	me->_updateSensorStates(list);
}

void SensorService::_temperatureAction(SensorService* me)
{
	for (auto kv : me->_tempSensorList)
	{
		const auto val = me->_tempreatureValueRequest(kv.first);

		FB_DEBUG_TAG_LOG("Temperature action got %f for id %llX", val, kv.first);

		me->_updateTemperatureValue(kv.first, val);
	}
}
