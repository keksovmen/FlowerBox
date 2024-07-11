#include "fb_box.hpp"



using namespace fb;
using namespace box;



Box::Box(const std::string& name,
	const std::string& version,
	id::UniqueId uniqueId)
	: _uniqueId(uniqueId), _name(name), _version(version)
{

}

std::string Box::toJson() const
{
	//TODO: use cJSON and add all missing fields from android app
	return "{name=\"" + _name + "\",version=\"" + _version + "\", unique_id=" + std::to_string(_uniqueId) + "}";
}