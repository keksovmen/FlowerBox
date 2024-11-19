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
	// const auto* prop = _box.addProperty(std::make_unique<PropertyInt>(
	// 	Tid::PROPERTY_SENSOR_PERIOD_GLOBAL,
	// 	[](int val){
	// 		global::getSensorService()->setTimerPeriod(val * 1000);
	// 		return true;
	// 	},
	// 	3));//TODO: fix it, service has not been stated here so timer is not exists
		
	// _box.addPropertyDependency(prop->getId());
}

void BoxService::handleEvent(const event::Event& event)
{
	if(event.groupId == event::EventGroup::SENSOR){
		if(event.eventId == sensor::SensorEvent::SENSOR_INITIALIZED){


		}else if(event.eventId == sensor::SensorEvent::SENSOR_VALUE_CHANGED){
			//TODO: move it somehwere else maybe?
			const auto* sen = reinterpret_cast<sensor::TemperatureSensor*>(event.data);
			if(sen){
				_storage.addSensorValue(reinterpret_cast<int>(sen), sen->getValue());
			}

			// for(int i = 0; i < sensorArray->getDeviceCount(); i++)
			// {
				// auto* s = sensorArray->getSensor(i);
			// }
			//для того чтобы хранить данные надо их както доставать нужен индекс по которому храним,
			//а так же перевод ид бокса в ид хранения для доставания
			

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

const char* BoxService::getName() const
{
	return "BoxService";
}
