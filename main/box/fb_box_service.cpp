#include "fb_box_service.hpp"

#include "fb_sensor_event.hpp"



using namespace fb;
using namespace box;



BoxService::BoxService(Box& box, sensor::SensorStorage& storage)
	: _box(box), _storage(storage)
{
	//TODO: put somehere else
	//TODO: current value take from SensorService
	//TODO: add property action cb
	_box.addProperty(std::make_unique<PropertyInt>(
		"Sensors period in sec",
		"Define period at which sensors are asked for data",
		0,
		[](int val){return true;},
		1, 1, 600
	));
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
			_box.addProperty(std::make_unique<PropertyString>(
				"Temp. description",
				"Defines sensor description",
				0,
				[](std::string val){return true;},
				"default value", 0, 300
			));

		}else if(event.eventId == sensor::SensorEvent::TEMPERATURE_SENSOR_VALUE_CHANGED){
			auto* sensor = reinterpret_cast<sensor::TemperatureSensor*>(event.data);
			_storage.addSensorValue(sensor->id, sensor->value);

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
