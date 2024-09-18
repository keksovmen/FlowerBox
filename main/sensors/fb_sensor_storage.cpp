#include "fb_sensor_storage.hpp"



using namespace fb;
using namespace sensor;



void SensorStorage::addSensorValue(TemperatureSensor::Id id, float value)
{
	//TODO: add check if last value was approximate the same ignore new alue
	_sensorData.pushValue(SensorStorageEntry{value, clock::currentTimeStamp()});
}

void SensorStorage::addSensorState(TemperatureSensor::Id id, bool state)
{

}

SensorStorage::Iterator SensorStorage::getSensorValues(TemperatureSensor::Id id, clock::Timestamp from) const
{
	return _sensorData.findValueIndex([&from](const SensorStorageEntry& e){
		return e.timestamp > from;
	});
}

SensorStorage::Iterator SensorStorage::getSensorValuesEnd() const
{
	return _sensorData.end();
}

SensorStorage::Buffer& SensorStorage::_getSensorValueBuffer(TemperatureSensor::Id id)
{
	return _sensorData;
}
