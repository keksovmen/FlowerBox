#include "fb_sensor_store_service.hpp"

#include "fb_tid.hpp"
#include "fb_sensor_event.hpp"
#include "fb_globals.hpp"
#include "fb_sensor_aht20.hpp"
#include "fb_project_core.hpp"



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
			
			//if no clock is init just skip
			//TODO: for project that do not have clock is bad design
			if(project::getInfo().requiresTime && !clock::isTimeSynced())
			{
				return;
			}

			_handleChangeEvent(reinterpret_cast<sensor::SensorIface*>(event.data));

			
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

void SensorStoreService::registerSensor(const SensorStoreCfg& sens)
{
	_regedSensors.push_back(sens);
}

void SensorStoreService::_handleChangeEvent(sensor::SensorIface* sensor)
{
	//need a way to define what sensor it is 
	if(!_isSupportedSensor(sensor)){
		return;
	}

	for(int i = 0; i < sensor->getIndexCount(); i++){
		const int address = reinterpret_cast<int>(sensor) + i;

		auto lastVal = _storage.getSensorLastValue(address);
		//check if initial value exists if not then add
		//and check if inital exists compare with last using mean square function
		if(!lastVal ||
			(std::pow(sensor->getValueIndexed(i) - lastVal.value(), 2) > std::pow(_mapSensorToPrecision(sensor, i), 2)))
		{
			_storage.addSensorValue(address, sensor->getValueIndexed(i));
		}
	}

	// {
	// 	const auto* sen = dynamic_cast<sensor::TemperatureSensor*>(reinterpret_cast<sensor::SensorIface*>(event.data));
	// 	auto last = _storage.getSensorLastValue(reinterpret_cast<int>(sen));
	// 	if(sen && (std::pow(sen->getValue() - , 2) < std::pow(_mapSensorToPrecision(sen), 2))){
	// 		_storage.addSensorValue(reinterpret_cast<int>(sen), sen->getValue());
	// 	}
	// }

	// {
	// 	const auto* sen = dynamic_cast<sensor::SensorAht20*>(reinterpret_cast<sensor::SensorIface*>(event.data));
	// 	if(sen){
	// 		_storage.addSensorValue(reinterpret_cast<int>(sen), sen->getTemperature());
	// 		//TODO: comment offset
	// 		_storage.addSensorValue(reinterpret_cast<int>(sen) + 1, sen->getHumidity());
	// 	}
	// }
}

bool SensorStoreService::_isSupportedSensor(const sensor::SensorIface* sensor) const
{
	return std::find_if(
		_regedSensors.begin(),
		_regedSensors.end(),
		[sensor](auto cfg){return cfg.sens == sensor;}
		) != _regedSensors.end();
	// if(dynamic_cast<const sensor::TemperatureSensor*>(sensor)){
	// 	return true;
	// }

	// if(dynamic_cast<const sensor::SensorAht20*>(sensor)){
	// 	return true;
	// }

	// FB_DEBUG_LOG_W_OBJ("Unsupported sensor for saving: %s", typeid(sensor).name());

	// return false;
}

float SensorStoreService::_mapSensorToPrecision(const sensor::SensorIface* sensor, int valueIndex) const
{
	//TODO: demove dynamic_cast and typeid putting everything in to sensor class
	// if(dynamic_cast<const sensor::TemperatureSensor*>(sensor)){
	// 	return 0.5f;
	// }

	// if(dynamic_cast<const sensor::SensorAht20*>(sensor)){
	// 	return valueIndex == sensor::SensorAht20::ValueTemperatureIndex ?
	// 		0.5f : 1.5f;
	// }


	// return 0.0f;

	auto iter = std::find_if(
		_regedSensors.begin(),
		_regedSensors.end(),
		[sensor](auto cfg){return cfg.sens == sensor;});
	assert(iter != _regedSensors.end());

	return std::invoke(iter->mapIndexToPrecision, valueIndex);
}