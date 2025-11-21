#include "fb_sensor_storage.hpp"

#include <cassert>



using namespace fb;
using namespace sensor;



void SensorStorage::addSensorValue(int address, float value)
{
	_addAddres(address);

	//если последнее равно текущему то игнорируем
	if(getSensorLastValue(address).value_or(-255.0f) == value)
	{
		return;
	}

	//науверенных тыкаем в указатель, ибо мы его точно получили через создание _addAddres()
	_getSensorValueBuffer(address)->pushValue(SensorStorageEntry{value, clock::currentTimeStamp()});
}

// void SensorStorage::addSensorState(int address, bool state)
// {

// }

SensorStorage::Iterator SensorStorage::getSensorValues(int address, clock::Timestamp from) const
{
	auto* buffer = _getSensorValueBuffer(address);
	if(!buffer){
		return _sensorData[_MISSING_BUFFER_INDEX].end();
	}

	return buffer->findValueIndex([&from](const SensorStorageEntry& e){
		return e.timestamp > from;
	});
}

SensorStorage::Iterator SensorStorage::getSensorValuesEnd(int address) const
{
	auto* buffer = _getSensorValueBuffer(address);
	if(!buffer){
		return _sensorData[_MISSING_BUFFER_INDEX].end();
	}

	return buffer->end();
}

std::optional<float> SensorStorage::getSensorLastValue(int address) const
{
	const int index = _mapAddresToIndex(address);
	if(index == SensorStorage::_ILLEGAL_INDEX){
		return {};
	}

	auto* buff = _getSensorValueBuffer(address);
	if(buff == nullptr){
		return {};
	}

	auto iter = buff->last();
	if(!iter){
		return {};
	}

	return {iter->value};
}

SensorStorage::Buffer* SensorStorage::_getSensorValueBuffer(int address)
{
	const int index = _mapAddresToIndex(address);
	return index == SensorStorage::_ILLEGAL_INDEX ? nullptr : &_sensorData.at(index);
	// assert(index != SensorStorage::_ILLEGAL_INDEX);
}

const SensorStorage::Buffer* SensorStorage::_getSensorValueBuffer(int address) const
{
	const int index = _mapAddresToIndex(address);
	return index == SensorStorage::_ILLEGAL_INDEX ? nullptr : &_sensorData.at(index);
}

int SensorStorage::_mapAddresToIndex(int address) const
{
	return (_addressMap.find(address) != _addressMap.end()) ? _addressMap.at(address) : SensorStorage::_ILLEGAL_INDEX;
}

int SensorStorage::_addAddres(int address)
{
	if(_addressMap.find(address) != _addressMap.end()){
		return _addressMap.at(address);
	}

	assert(_addressMap.size() < _sensorData.size());

	const int result = _addressMap.size();
	_addressMap[address] = result;

	return result;
}