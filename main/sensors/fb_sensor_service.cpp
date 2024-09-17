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



const char* SensorService::getName()
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
	// auto view = _sensors | std::views::values | std::views::as_rvalue;

	// return {std::ranges::begin(view), std::ranges::end(view)};
	return _sensors;
}

void SensorService::_onPull()
{
	for(auto* s : _sensors){
		if(s->isInit()){
			if(s->update()){
				_dropEvent(SensorEvent::SENSOR_VALUE_CHANGED, s);
			}else{
				_dropEvent(SensorEvent::SENSOR_LOST, s);
			}
		}else{
			if(s->init()){
				_dropEvent(SensorEvent::SENSOR_INITIALIZED, s);
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