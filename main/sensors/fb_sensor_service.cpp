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



const char* SensorService::getName() const
{
	return "SensorService";
}

void SensorService::addSensor(SensorIface* entry)
{
	//TODO: MUTEX TAKE HERE
	_sensors.push_back(entry);
}

std::vector<SensorIface*> SensorService::getSensors() const
{
	return _sensors;
}

void SensorService::_onPull()
{
	//инициализируем тех кто не еще не инициализирован
	auto notInitFilter = [](auto* val){return val->isInit() == false;};
	for(auto* s : _sensors | std::views::filter(notInitFilter)){
		if(s->init()){
			_dropEvent(SensorEvent::SENSOR_INITIALIZED, s);
		}
	}

	//обновляем данные всех, кто инициализирован
	auto initFilter = [&notInitFilter](auto* val){return !notInitFilter(val);};
	for(auto* s : _sensors | std::views::filter(initFilter)){
		if(s->update()){
			_dropEvent(SensorEvent::SENSOR_VALUE_CHANGED, s);
		}else{
			if(!s->isInit()){
				_dropEvent(SensorEvent::SENSOR_LOST, s);
			}
		}
	}
}

void SensorService::_dropEvent(SensorEvent e, SensorIface* data)
{
	global::getEventManager()->pushEvent(event::Event{
		event::EventGroup::SENSOR,
		static_cast<int>(e),
		data
	});	
}