#include "fb_switch.hpp"

#include <algorithm>

#include "cJSON.h"



using namespace fb;
using namespace box;



Switch::Switch(const std::string& name,
	const std::string& description,
	int id,
	int tid,
	bool state,
	std::vector<int> dependentProperties,
	std::vector<int> dependentSensors)
	: _name(name), _description(description),
	_id(id), _tid(tid),
	_state(state),
	_dependentProperties(dependentProperties),
	_dependentSensors(dependentSensors)
{

}

std::string Switch::toJson() const
{
	cJSON* obj = cJSON_CreateObject();
	cJSON_AddStringToObject(obj, "name", _name.c_str());
	cJSON_AddStringToObject(obj, "description", _description.c_str());
	cJSON_AddNumberToObject(obj, "id", _id);
	cJSON_AddNumberToObject(obj, "tid", _tid);
	cJSON_AddNumberToObject(obj, "state", _state ? 1 : 0);
	cJSON_AddItemToObject(obj, "property_ids", cJSON_CreateIntArray(_dependentProperties.data(), _dependentProperties.size()));
	cJSON_AddItemToObject(obj, "sensors_ids", cJSON_CreateIntArray(_dependentSensors.data(), _dependentSensors.size()));

	std::string result(cJSON_PrintUnformatted(obj));

	cJSON_Delete(obj);

	return result;
}

int Switch::getId() const
{
	return _id;
}

void Switch::setId(int id)
{
	_id = id;
}
