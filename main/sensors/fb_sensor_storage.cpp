#include "fb_sensor_storage.hpp"



using namespace fb;
using namespace sensor;



void SensorStorage::addSensorValue(TemperatureSensor::Id id, float value)
{

	//если полседнее значение точно такое же, то не добавляем
	if(_sensorData[id].last() && _sensorData[id].last()->value == value){
		return;
	}

	_sensorData[id].pushValue(SensorStorageEntry{value, clock::currentTimeStamp()});
}

void SensorStorage::addSensorState(TemperatureSensor::Id id, bool state)
{

}

SensorStorage::Iterator SensorStorage::getSensorValues(TemperatureSensor::Id id, clock::Timestamp from) const
{
	return _sensorData[id].findValueIndex([&from](const SensorStorageEntry& e){
		return e.timestamp > from;
	});
}

SensorStorage::Iterator SensorStorage::getSensorValuesEnd(TemperatureSensor::Id id) const
{
	return _sensorData[id].end();
}

SensorStorage::Buffer& SensorStorage::_getSensorValueBuffer(TemperatureSensor::Id id)
{
	return _sensorData[id];
}
