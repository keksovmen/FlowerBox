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
	const auto* prop = _box.addProperty(std::make_unique<PropertyInt>(
		Tid::PROPERTY_SENSOR_PERIOD_GLOBAL,
		[](int val){
			global::getSensorService()->setTimerPeriod(val * 1000);
			return true;
		},
		3));//TODO: fix it, service has not been stated here so timer is not exists
		
	_box.addPropertyDependency(prop->getId());
}

void BoxService::handleEvent(const event::Event& event)
{
	if(event.groupId == event::EventGroup::SENSOR){
		if(event.eventId == sensor::SensorEvent::SENSOR_INITIALIZED){


		}else if(event.eventId == sensor::SensorEvent::SENSOR_VALUE_CHANGED){
			//TODO: move it somehwere else maybe?
			auto* sensorArray = reinterpret_cast<sensor::TemperatureSensorArrayI*>(event.data);
			for(int i = 0; i < sensorArray->getDeviceCount(); i++)
			{
				auto* s = sensorArray->getSensor(i);
				_storage.addSensorValue(i, s->getValue());
			}

			// for check working purposes only
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
