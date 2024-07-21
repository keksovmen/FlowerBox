#include "fb_data_entry.hpp"

#include "cJSON.h"
#include "fb_time.hpp"



using namespace fb;
using namespace box;



DataEntry::DataEntry(float value)
	: _value(value), _timestamp(clock::currentTime())
{

}

DataEntry::DataEntry(float value, clock::Timestamp timestamp)
	: _value(value), _timestamp(timestamp)
{

}

std::string DataEntry::toJson() const
{
	cJSON* obj = cJSON_CreateObject();
	cJSON_AddNumberToObject(obj, "value", _value);
	cJSON_AddNumberToObject(obj, "timestamp", _timestamp);
	
	std::string result(cJSON_PrintUnformatted(obj));

	cJSON_Delete(obj);

	return result;
}
