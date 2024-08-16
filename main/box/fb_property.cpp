#include "fb_property.hpp"

#include "cJSON.h"



using namespace fb;
using namespace box;




void PropertyIface::setId(int id)
{
	_id = id;
}

int PropertyIface::getId() const
{
	return _id;
}



template<class T>
PropertyBase<T>::PropertyBase(
			const std::string& name,
			const std::string& description,
			Tid tid,
			const ActionSet& action,
			T value)
	: _name(name), _description(description),
	_tid(tid), _action(action),
	_value(value)
{

}

//TODO: create cJson and append key value as str
template<class T>
std::string PropertyBase<T>::toJson() const
{
	cJSON* obj = cJSON_CreateObject();
	cJSON_AddStringToObject(obj, "name", _name.c_str());
	cJSON_AddStringToObject(obj, "description", _description.c_str());
	cJSON_AddStringToObject(obj, "value_type", _getValueType().c_str());
	cJSON_AddNumberToObject(obj, "id", getId());
	cJSON_AddNumberToObject(obj, "tid", _tid);
	cJSON_AddRawToObject(obj, "min_value", _minValueToString().c_str());
	cJSON_AddRawToObject(obj, "max_value", _maxValueToString().c_str());
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
			const std::string& name,
			const std::string& description,
			Tid tid,
			const ActionSet& action,
			int value,
			int minValue,
			int maxValue)
	: PropertyBase(name, description, tid, action, value),
	_minValue(minValue), _maxValue(maxValue) 
{

}

std::pair<bool, int> PropertyInt::_strToValue(const std::string& str) const
{
	return {true, std::atoi(str.c_str())};
}

std::string PropertyInt::_valueToString(int val) const
{
	return std::to_string(val);
}

std::string PropertyInt::_minValueToString() const
{
	return std::to_string(_minValue);
}

std::string PropertyInt::_maxValueToString() const
{
	return std::to_string(_maxValue);
}

std::string PropertyInt::_getValueType() const
{
	return "int";
}



PropertyString::PropertyString(
			const std::string& name,
			const std::string& description,
			Tid tid,
			const ActionSet& action,
			const std::string& value,
			int minLength,
			int maxLength)
	: PropertyBase(name, description, tid, action, value),
	_minLength(minLength), _maxLength(maxLength) 
{

}

std::pair<bool, std::string> PropertyString::_strToValue(const std::string& str) const
{
	return {true, str};
}

std::string PropertyString::_valueToString(std::string val) const
{
	return val;
}

std::string PropertyString::_minValueToString() const
{
	return std::to_string(_minLength);
}

std::string PropertyString::_maxValueToString() const
{
	return std::to_string(_maxLength);
}

std::string PropertyString::_getValueType() const
{
	return "str";
}


Property::Property(const std::string& name,
	const std::string& description,
	const std::string& valueType,
	int id,
	int tid,
	double minValue,
	double maxValue,
	double value)
	: _name(name), _description(description), _valueType(valueType),
	_id(id), _tid(tid),
	_minValue(minValue), _maxValue(maxValue), _value(value)
{

}

std::string Property::toJson() const
{
	cJSON* obj = cJSON_CreateObject();
	cJSON_AddStringToObject(obj, "name", _name.c_str());
	cJSON_AddStringToObject(obj, "description", _description.c_str());
	cJSON_AddStringToObject(obj, "value_type", _valueType.c_str());
	cJSON_AddNumberToObject(obj, "id", _id);
	cJSON_AddNumberToObject(obj, "tid", _tid);
	cJSON_AddNumberToObject(obj, "min_value", _minValue);
	cJSON_AddNumberToObject(obj, "max_value", _maxValue);
	cJSON_AddNumberToObject(obj, "value", _value);

	std::string result(cJSON_PrintUnformatted(obj));

	cJSON_Delete(obj);

	return result;
}

int Property::getId() const
{
	return _id;
}

void Property::setId(int id)
{
	_id = id;
}
