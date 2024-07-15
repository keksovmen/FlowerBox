#include "fb_sensor.hpp"

#include "cJSON.h"



using namespace fb;
using namespace box;



Sensor::Sensor(const std::string& name,
	const std::string& description,
	const std::string& valueType,
	int id,
	int tid,
	double minValue,
	double maxValue)
	: _name(name), _description(description), _valueType(valueType),
	_id(id), _tid(tid),
	_minValue(minValue), _maxValue(maxValue)
{

}

std::string Sensor::toJson() const
{
	cJSON* obj = cJSON_CreateObject();
	cJSON_AddStringToObject(obj, "name", _name.c_str());
	cJSON_AddStringToObject(obj, "description", _description.c_str());
	cJSON_AddStringToObject(obj, "value_type", _valueType.c_str());
	cJSON_AddNumberToObject(obj, "id", _id);
	cJSON_AddNumberToObject(obj, "tid", _tid);
	cJSON_AddNumberToObject(obj, "min_value", _minValue);
	cJSON_AddNumberToObject(obj, "max_value", _maxValue);

	std::string result(cJSON_PrintUnformatted(obj));

	cJSON_Delete(obj);

	return result;
}

int Sensor::getId() const
{
	return _id;
}

void Sensor::setId(int id)
{
	_id = id;
}
