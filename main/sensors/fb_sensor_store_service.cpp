#include "fb_sensor_store_service.hpp"

#include "fb_tid.hpp"
#include "fb_sensor_event.hpp"
#include "fb_globals.hpp"
#include "fb_sensor_aht20.hpp"



using namespace fb;
using namespace sensor;



SensorStoreService::SensorStoreService(sensor::SensorStorage& storage)
	: _storage(storage)
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

void SensorStoreService::handleEvent(const event::Event& event)
{
	if(event.groupId == event::EventGroup::SENSOR){
		if(event.eventId == sensor::SensorEvent::SENSOR_INITIALIZED){


		}else if(event.eventId == sensor::SensorEvent::SENSOR_VALUE_CHANGED){
			//need a way to define what sensor it is 
			{
				const auto* sen = dynamic_cast<sensor::TemperatureSensor*>(reinterpret_cast<sensor::SensorIface*>(event.data));
				if(sen){
					_storage.addSensorValue(reinterpret_cast<int>(sen), sen->getValue());
				}
			}

			{
			const auto* sen = dynamic_cast<sensor::SensorAht20*>(reinterpret_cast<sensor::SensorIface*>(event.data));
				if(sen){
					_storage.addSensorValue(reinterpret_cast<int>(sen), sen->getTemperature());
					//TODO: comment offset
					_storage.addSensorValue(reinterpret_cast<int>(sen) + 1, sen->getHumidity());
				}
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

const char* SensorStoreService::getName() const
{
	return "SensorStoreService";
}
