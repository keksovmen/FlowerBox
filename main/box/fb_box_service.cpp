#include "fb_box_service.hpp"

#include "fb_sensor_event.hpp"



using namespace fb;
using namespace box;



BoxService::BoxService(Box& box)
	: _box(box)
{
	//TODO: put somehere else
	//TODO: current value take from SensorService
	//TODO: add property action cb
	_box.addProperty(Property{
		"Sensors period in sec", "Define period at which sensors are asked for data", "int", 0, 0, 1, 600, 0
	});
}

void BoxService::handleEvent(const event::Event& event)
{
	if(event.groupId == event::EventGroup::SENSOR){
		if(event.eventId == sensor::SensorEvent::TEMPERATURE_SENSOR_DETECTED){
			//добавить устройство в Box
			_box.addSensor(Sensor{
				"Temp. DS18B20", "Description", "float", 0, 0, -50.0, 100.0
			});

			//добвить свойства: период опроса, название датчика
			_box.addProperty(Property{
				"Temp. description", "Defines sensor description", "str", 0, 0, 0, 128, 0
			});
		}
	}
}

const char* BoxService::getName()
{
	return "BoxService";
}
