#include "fb_box_service.hpp"

#include "fb_tid.hpp"
#include "fb_sensor_event.hpp"
#include "fb_globals.hpp"



using namespace fb;
using namespace box;



BoxService::BoxService(Box& box, sensor::SensorStorage& storage)
	: _box(box), _storage(storage)
{
	//TODO: put somehere else
	//TODO: add property action cb
	_box.addProperty(std::make_unique<PropertyInt>(
		Tid::PROPERTY_SENSOR_PERIOD_GLOBAL,
		[](int val){
			global::getSensorService()->setTimerPeriod(val * 1000);
			return true;
		},
		global::getSensorService()->getTimerPeriod() / 1000));
}

void BoxService::handleEvent(const event::Event& event)
{
	if(event.groupId == event::EventGroup::SENSOR){
		if(event.eventId == sensor::SensorEvent::SENSOR_INITIALIZED){
			//добавить устройство в Box
			auto& sen = _box.addSensor(Sensor{Tid::SENSOR_DS18B20});

			//добвить свойства: период опроса, название датчика
			const auto* prop = _box.addProperty(std::make_unique<PropertyString>(
				"Temp. sensor " + std::to_string(sen.getId()) + " description",
				"Defines sensor description with id " + std::to_string(sen.getId()),
				Tid::PROPERTY_SENSOR_DESCRIPTION,
				[](std::string val){return true;},
				sen.getDescription()));
			
			sen.addPropertyDependency(prop->getId());

		}else if(event.eventId == sensor::SensorEvent::SENSOR_VALUE_CHANGED){
			//TODO: fix it, now it is wrong pointer type
			// auto* sensor = reinterpret_cast<sensor::TemperatureSensor*>(event.data);
			// _storage.addSensorValue(sensor->id, sensor->value);

			//for check working purposes only
			// {
			// 	auto iter = _storage.getSensorValues(sensor->id, 0);
			// 	if(iter){
			// 		for(; iter != _storage.getSensorValuesEnd(); ++iter){
			// 			ESP_LOGI(getName(), "Sensor data entry: at %llu, %.2f", (*iter).timestamp, (*iter).value);
			// 		}
			// 	}else{
			// 		ESP_LOGI(getName(), "No values present in storage");
			// 	}
			// }
		}
	}
}

const char* BoxService::getName()
{
	return "BoxService";
}
