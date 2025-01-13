#include "fb_sensor_storage.hpp"



using namespace fb;
using namespace sensor;



void SensorStorage::addSensorValue(int address, float value)
{
	_addAddres(address);

	//если полседнее значение точно такое же, то не добавляем
	if(_getSensorValueBuffer(address).last() && _getSensorValueBuffer(address).last()->value == value){
		return;
	}

	_getSensorValueBuffer(address).pushValue(SensorStorageEntry{value, clock::currentTimeStamp()});
}

// void SensorStorage::addSensorState(int address, bool state)
// {

// }

SensorStorage::Iterator SensorStorage::getSensorValues(int address, clock::Timestamp from) const
{
	return _getSensorValueBuffer(address).findValueIndex([&from](const SensorStorageEntry& e){
		return e.timestamp > from;
	});
}

SensorStorage::Iterator SensorStorage::getSensorValuesEnd(int address) const
{
	return _getSensorValueBuffer(address).end();
}

SensorStorage::Buffer& SensorStorage::_getSensorValueBuffer(int address)
{
	const int index = _mapAddresToIndex(address);
	assert(index != SensorStorage::_ILLEGAL_INDEX);

	return _sensorData.at(index);
}

const SensorStorage::Buffer& SensorStorage::_getSensorValueBuffer(int address) const
{
	const int index = _mapAddresToIndex(address);
	assert(index != SensorStorage::_ILLEGAL_INDEX);

	return _sensorData.at(index);
}

int SensorStorage::_mapAddresToIndex(int address) const
{
	return _addressMap.contains(address) ? _addressMap.at(address) : SensorStorage::_ILLEGAL_INDEX;
}

int SensorStorage::_addAddres(int address)
{
	if(_addressMap.contains(address)){
		return _addressMap.at(address);
	}

	assert(_addressMap.size() < _sensorData.size());

	const int result = _addressMap.size();
	_addressMap[address] = result;

	return result;
}