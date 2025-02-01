#include "fb_data_entry.hpp"

#include "fb_dynamic_str_constructor.hpp"

#include "cJSON.h"



using namespace fb;
using namespace box;



DataEntry::DataEntry(float value)
	: _value(value), _timestamp(clock::currentTimeStamp())
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
	
	std::string result(util::DynamicStrConstructor(cJSON_PrintUnformatted(obj)));

	cJSON_Delete(obj);

	return result;
}
