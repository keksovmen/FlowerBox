#include "fb_box.hpp"

#include <algorithm>

#include "cJSON.h"

#include "fb_debug.hpp"



using namespace fb;
using namespace box;



static const char* TAG = "BoxObject";



Box::Box(const std::string& name,
	const std::string& version,
	id::UniqueId uniqueId)
	: _uniqueId(uniqueId), _name(name), _version(version)
{

}

std::string Box::toJson() const
{
	// int ids[] = {0, 1, 2, 3};
	cJSON* obj = cJSON_CreateObject();
	cJSON_AddStringToObject(obj, "name", _name.c_str());
	cJSON_AddStringToObject(obj, "version", _version.c_str());
	cJSON_AddNumberToObject(obj, "unique_id", _uniqueId);

	//TODO: use cJSON and add all missing fields from android app
	// cJSON_AddItemToObject(obj, "switch_ids", cJSON_CreateIntArray(ids, 4));
	// cJSON_AddItemToObject(obj, "sensor_ids", cJSON_CreateIntArray(ids, 4));

	std::vector<int> ids;
	std::for_each(_properties.begin(), _properties.end(), [&ids](const auto& p){ids.push_back(p.getId());});
	cJSON_AddItemToObject(obj, "properties_ids", cJSON_CreateIntArray(ids.data(), _properties.size()));

	ids.clear();
	std::for_each(_sensors.begin(), _sensors.end(), [&ids](const auto& p){ids.push_back(p.getId());});
	cJSON_AddItemToObject(obj, "sensor_ids", cJSON_CreateIntArray(ids.data(), _sensors.size()));

	std::string result(cJSON_PrintUnformatted(obj));

	cJSON_Delete(obj);

	return result;
}

void Box::addProperty(const Property& prop)
{
	auto iter = std::find_if(_properties.begin(), _properties.end(),
		[prop](const Property& left){return left.getId() == prop.getId();});
	
	if(iter != _properties.end()){
		FB_DEBUG_TAG_LOG_W("Failed to add property with id %d, it is already exist", prop.getId());
		return;
	}

	_properties.push_back(prop);

	FB_DEBUG_TAG_LOG_W("Added a property with id %d", prop.getId());
}

void Box::addSensor(const Sensor& sensor)
{
	auto iter = std::find_if(_sensors.begin(), _sensors.end(),
		[sensor](const Sensor& left){return left.getId() == sensor.getId();});
	
	if(iter != _sensors.end()){
		FB_DEBUG_TAG_LOG_W("Failed to add sensor with id %d, it is already exist", sensor.getId());
		return;
	}

	_sensors.push_back(sensor);

	FB_DEBUG_TAG_LOG_W("Added a sensor with id %d", sensor.getId());
}

const Property* Box::getProperty(int id) const
{
	auto iter = std::find_if(_properties.begin(), _properties.end(),
		[id](const Property& left){return left.getId() == id;});
	
	if(iter == _properties.end()){
		return nullptr;
	}

	return &(*iter);
}

const Sensor* Box::getSensor(int id) const
{
	auto iter = std::find_if(_sensors.begin(), _sensors.end(),
		[id](const Sensor& left){return left.getId() == id;});
	
	if(iter == _sensors.end()){
		return nullptr;
	}

	return &(*iter);
}
