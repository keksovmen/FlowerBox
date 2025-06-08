#include "fb_switch.hpp"

#include <algorithm>

#include "fb_dynamic_str_constructor.hpp"

#include "cJSON.h"



using namespace fb;
using namespace box;



Switch::Switch(Tid tid,
	std::vector<int> dependentProperties,
	std::vector<int> dependentSensors,
	const ReadStateCb& stateCb)
	: ObjectStaticTid(tid),
	_stateCb(stateCb),
	_dependentSensors(dependentSensors)
{
	std::for_each(dependentProperties.begin(), dependentProperties.end(),
		[this](int val){
			addPropertyDependency(val);
		});
}

Switch::Switch(Tid tid, const ReadStateCb& stateCb)
	: ObjectStaticTid(tid),
	_stateCb(stateCb)
{

}

std::string Switch::toJson() const
{
	cJSON* obj = cJSON_CreateObject();
	cJSON_AddStringToObject(obj, "name", getName().c_str());
	cJSON_AddStringToObject(obj, "description", getDescription().c_str());
	cJSON_AddNumberToObject(obj, "id", getId());
	cJSON_AddNumberToObject(obj, "tid", static_cast<int>(getTid()));
	cJSON_AddNumberToObject(obj, "state", currentState() ? 1 : 0);
	cJSON_AddItemToObject(obj, "property_ids", cJSON_CreateIntArray(getPropertyDependencies().data(), getPropertyDependencies().size()));
	cJSON_AddItemToObject(obj, "sensors_ids", cJSON_CreateIntArray(_dependentSensors.data(), _dependentSensors.size()));

	std::string result(util::DynamicStrConstructor(cJSON_PrintUnformatted(obj)));

	cJSON_Delete(obj);

	return result;
}

void Switch::addSensorDependency(int sensorId)
{
	_dependentSensors.push_back(sensorId);
}

bool Switch::currentState() const
{
	return std::invoke(_stateCb);
}