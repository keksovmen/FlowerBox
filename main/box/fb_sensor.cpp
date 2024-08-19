#include "fb_sensor.hpp"

#include "cJSON.h"



using namespace fb;
using namespace box;



Sensor::Sensor(Tid tid)
	: ObjectStaticTid(tid)
{

}

std::string Sensor::toJson() const
{
	cJSON* obj = cJSON_CreateObject();
	cJSON_AddStringToObject(obj, "name", getName().c_str());
	cJSON_AddStringToObject(obj, "description", getDescription().c_str());
	cJSON_AddStringToObject(obj, "value_type", getValueType().c_str());
	cJSON_AddNumberToObject(obj, "id", getId());
	cJSON_AddNumberToObject(obj, "tid", static_cast<double>(getTid()));
	cJSON_AddRawToObject(obj, "min_value", getMinValueStr().c_str());
	cJSON_AddRawToObject(obj, "max_value", getMaxValueStr().c_str());

	std::string result(cJSON_PrintUnformatted(obj));

	cJSON_Delete(obj);

	return result;
}