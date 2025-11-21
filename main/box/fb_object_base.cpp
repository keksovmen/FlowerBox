#include "fb_object_base.hpp"

#include <algorithm>
#include <cassert>



using namespace fb;
using namespace box;



ObjectIface::ObjectIface()
{

}

ObjectIface::ObjectIface(
	Tid tid,
	std::string name,
	std::string description,
	std::string valueType,
	std::string minValue,
	std::string maxValue
)
	: _tid(tid),
	_name(name), _description(description),
	_valueType(valueType),
	_minValue(minValue), _maxValue(maxValue)
{

}

void ObjectIface::setTid(Tid tid)
{
	_tid = tid;
}

Tid ObjectIface::getTid() const
{
	return _tid;
}

void ObjectIface::setId(int id)
{
	_id = id;
}

int ObjectIface::getId() const
{
	return _id;
}

void ObjectIface::setName(const std::string& val)
{
	_name = val;
}

void ObjectIface::setDescription(const std::string& val)
{
	_description = val;
}

void ObjectIface::setValueType(const std::string& val)
{
	_valueType = val;
}

void ObjectIface::setMinValueStr(const std::string& val)
{
	_minValue = val;
}

void ObjectIface::setMaxValueStr(const std::string& val)
{
	_maxValue = val;
}

const std::string& ObjectIface::getName() const
{
	return _name;
}

const std::string& ObjectIface::getDescription() const
{
	return _description;
}

const std::string& ObjectIface::getValueType() const
{
	return _valueType;
}

const std::string& ObjectIface::getMinValueStr() const
{
	return _minValue;
}

const std::string& ObjectIface::getMaxValueStr() const
{
	return _maxValue;
}

void ObjectIface::addPropertyDependency(int propertyId)
{
	assert(std::find(_dependentProperties.begin(), _dependentProperties.end(), propertyId) == _dependentProperties.end());

	_dependentProperties.push_back(propertyId);
}

const ObjectIface::PropertyArray& ObjectIface::getPropertyDependencies() const
{
	return _dependentProperties;
}



ObjectStaticTid::ObjectStaticTid(Tid tid)
	: ObjectIface(
		tid,
		tidToName(tid), tidToDescription(tid),
		tidToValueType(tid),
		tidToMinValue(tid), tidToMaxValue(tid)
	)
{
	
}