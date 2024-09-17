#include "fb_tid.hpp"

#include <unordered_map>



using namespace fb;
using namespace box;




struct _TidHolder
{
	std::string name;
	std::string description;
	std::string valueType;
	std::string minValue;
	std::string maxValue;
};



static const std::unordered_map<Tid, _TidHolder> _map = {
	{Tid::SENSOR_DS18B20, {"DS18B20", "", "float", "-100.0", "100.0"}},
	{Tid::SWITCH_LIGHT, {"Light switch", "", "bool", "0", "1"}},
	{Tid::PROPERTY_SENSOR_PERIOD_GLOBAL, {"Sensor mesure period", "measured in SEC", "int", "1", "600"}},
	{Tid::PROPERTY_SENSOR_DESCRIPTION, {"Changes description", "", "int", "0", "300"}},
	{Tid::PROPERTY_SWITCH_FORSE, {"Forses on state", "", "bool", "0", "1"}}
};



const char* box::tidToName(Tid tid)
{
	return _map.at(tid).name.c_str();
}

const char* box::tidToDescription(Tid tid)
{
	return _map.at(tid).description.c_str();
}

const char* box::tidToValueType(Tid tid)
{
	return _map.at(tid).valueType.c_str();
}

const char* box::tidToMinValue(Tid tid)
{
	return _map.at(tid).minValue.c_str();
}

const char* box::tidToMaxValue(Tid tid)
{
	return _map.at(tid).maxValue.c_str();
}
