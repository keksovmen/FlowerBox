#include "fb_sensor_service.hpp"

#include <ranges>

#include "ds18b20.h"
#include "fb_globals.hpp"
#include "fb_pins.hpp"



#define _QUEUE_SIZE 5

#define _TASK_STACK_SIZE 2 * 1024
#define _TASK_PRIORITY 10



using namespace fb;
using namespace sensor;



static const char* TAG = "SensorService";



const char *SensorService::getName()
{
	return "SensorService";
}

void SensorService::start()
{
	// assert(ow_init(&_wireIface, gpio));
	assert(!_mutex);

	_mutex = xSemaphoreCreateMutex();
	assert(_mutex);

	_actionQueue = xQueueCreate(_QUEUE_SIZE, sizeof(SensorService::Action));
	assert(_actionQueue);

	_timerHndl = xTimerCreate(getName(), pdMS_TO_TICKS(3000), pdTRUE, this, &SensorService::_timer);
	assert(_timerHndl);

	auto err = xTimerStart(_timerHndl, portMAX_DELAY);
	assert(err = pdPASS);

	err = xTaskCreate(&SensorService::_task, getName(), _TASK_STACK_SIZE, this, _TASK_PRIORITY, &_taskHndl);
	assert(err == pdPASS);

	_sensors.push_back(new TempreatureSensorTest(pins::TEMPERATURE_SENSOR_PIN));
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

void SensorService::setTimerPeriod(int ms)
{
	assert(_timerHndl);
	
	xTimerChangePeriod(_timerHndl, pdMS_TO_TICKS(ms), portMAX_DELAY);
}

std::vector<TemperatureSensor> SensorService::getSensors() const
{
	auto view = _tempSensorList | std::views::values | std::views::as_rvalue;

	return {std::ranges::begin(view), std::ranges::end(view)};
}

void SensorService::_task(void *arg)
{
	SensorService *me = reinterpret_cast<SensorService*>(arg);

	for (;;)
	{
		SensorService::Action action;

		auto err = xQueueReceive(me->_actionQueue, &action, portMAX_DELAY);
		assert(err == pdPASS);

		std::invoke(action, me);
	}

	vTaskDelete(NULL);
}

void SensorService::_scanAction(SensorService* me)
{
	//TODO: change to range with filter
	std::for_each(me->_sensors.begin(), me->_sensors.end(),
		[me](auto* entry){
			if(!entry->isInit()){
				if(entry->init()){
					//need to drop event, that given sensor is init properly
					me->_dropEvent(SensorEvent::SENSOR_INITIALIZED, entry);
				}
			}
		}
	);
}

void SensorService::_temperatureAction(SensorService* me)
{
	//TODO: change to range with filter
	std::for_each(me->_sensors.begin(), me->_sensors.end(),
		[me](auto* entry){
			if(entry->isInit()){
				if(entry->update()){
					//TODO: some how determine was value changed or not
					me->_dropEvent(SensorEvent::SENSOR_VALUE_CHANGED, entry);
				}else{
					me->_dropEvent(SensorEvent::SENSOR_LOST, entry);
				}
			}
		}
	);
}

void SensorService::_timer(TimerHandle_t timer)
{
	SensorService* me = reinterpret_cast<SensorService*>(pvTimerGetTimerID(timer));
	me->forseScan();
	me->forseRead();
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
				// _dropEvent(SensorEvent::TEMPERATURE_SENSOR_DETECTED, &_tempSensorList[id]);
			}
		}
		else
		{
			_tempSensorList[id] = TemperatureSensor{true, 0.0f, id};
			// _dropEvent(SensorEvent::TEMPERATURE_SENSOR_DETECTED, &_tempSensorList[id]);
		}
	}

	//others devices must became disabled
	auto filter = [this, alive](const TemperatureSensor& val){return std::find(alive.begin(), alive.end(), val.id) == alive.end();};

	for(auto& sensor : _tempSensorList | std::views::values | std::views::filter(filter)){
		if(sensor.alive){
			sensor.alive = false;
			_dropEvent(SensorEvent::SENSOR_LOST, &_tempSensorList[sensor.id]);
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
		// _dropEvent(SensorEvent::TEMPERATURE_SENSOR_VALUE_CHANGED, &_tempSensorList[id]);
	}
}

void SensorService::_dropEvent(SensorEvent e, TemperatureSensor* data)
{
	global::getEventManager()->pushEvent(event::Event{
		event::EventGroup::SENSOR,
		static_cast<int>(e),
		data
	});	
}

void SensorService::_dropEvent(SensorEvent e, SensorIface* data)
{
	global::getEventManager()->pushEvent(event::Event{
		event::EventGroup::SENSOR,
		static_cast<int>(e),
		data
	});	
}