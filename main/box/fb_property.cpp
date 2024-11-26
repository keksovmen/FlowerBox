#include "fb_property.hpp"

#include "cJSON.h"



using namespace fb;
using namespace box;



PropertyIface::PropertyIface(Tid tid)
	: ObjectStaticTid(tid)
{

}



template<class T>
PropertyBase<T>::PropertyBase(
			std::string name,
			std::string description,
			Tid tid,
			ActionSet action,
			T value)
	: PropertyIface(tid),
	_action(action), _value(value)
{
	setName(name);
	setDescription(description);
}

template<class T>
void PropertyBase<T>::setMinValueStr(const std::string& val)
{
	assert(0);
}

template<class T>
void PropertyBase<T>::setMaxValueStr(const std::string& val)
{
	assert(0);
}

//TODO: create cJson and append key value as str
template<class T>
std::string PropertyBase<T>::toJson() const
{
	cJSON* obj = cJSON_CreateObject();
	cJSON_AddStringToObject(obj, "name", getName().c_str());
	cJSON_AddStringToObject(obj, "description", getDescription().c_str());
	cJSON_AddStringToObject(obj, "value_type", getValueType().c_str());
	cJSON_AddNumberToObject(obj, "id", getId());
	cJSON_AddNumberToObject(obj, "tid", static_cast<double>(getTid()));
	cJSON_AddRawToObject(obj, "min_value", getMinValueStr().c_str());
	cJSON_AddRawToObject(obj, "max_value", getMaxValueStr().c_str());
	cJSON_AddStringToObject(obj, "value", _valueToString(_value).c_str());

	std::string result(cJSON_PrintUnformatted(obj));

	cJSON_Delete(obj);

	return result;
}

//TODO: call converter function and call calback if ok
template<class T>
bool PropertyBase<T>::setFromJson(const std::string& str)
{
	auto val = _strToValue(str);
	if(val.first){
		_value = val.second;
		std::invoke(_action, _value);

		return true;
	}

	return false;
}



PropertyInt::PropertyInt(
			std::string name,
			std::string description,
			Tid tid,
			ActionSet action,
			int value,
			int minValue,
			int maxValue)
	: PropertyBase(name, description, tid, action, value),
	_minValue(minValue), _maxValue(maxValue) 
{
	setValueType("int");
}

PropertyInt::PropertyInt(Tid tid, ActionSet action, int value)
	: PropertyBase(tidToName(tid), tidToDescription(tid), tid, action, value),
	_minValue(std::atoi(tidToMinValue(tid))),
	_maxValue(std::atoi(tidToMaxValue(tid)))
{
	setValueType("int");
}

std::pair<bool, int> PropertyInt::_strToValue(const std::string& str) const
{
	return {true, std::atoi(str.c_str())};
}

std::string PropertyInt::_valueToString(int val) const
{
	return std::to_string(val);
}



PropertyString::PropertyString(
			std::string name,
			std::string description,
			Tid tid,
			ActionSet action,
			std::string value)
	: PropertyBase(name, description, tid, action, value)
{
	setValueType("str");
}

PropertyString::PropertyString(Tid tid, ActionSet action, std::string value)
	: PropertyBase(tidToName(tid), tidToDescription(tid), tid, action, value)
{
	setValueType("str");
}

std::pair<bool, std::string> PropertyString::_strToValue(const std::string& str) const
{
	return {true, str};
}

std::string PropertyString::_valueToString(std::string val) const
{
	return val;
}



PropertyNone::PropertyNone(Tid tid, ActionSet action)
	: PropertyString(tid, action, "")
{
	setValueType("none");
}