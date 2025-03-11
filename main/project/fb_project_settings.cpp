#include "fb_project_settings.hpp"

#include <cstring>
#include <cstdlib>

#include "fb_settings.hpp"



#define _PARTITION "prj_set"

#define _KEY_LIGHT_START "l_on"
#define _KEY_LIGHT_END "l_off"
#define _KEY_FAN_SPEED "f_speed"
#define _KEY_FAN_LOW_TEMP "f_low_t"
#define _KEY_FAN_HIGH_TEMP "f_high_t"
#define _KEY_HEATER_LOW_TEMP "h_low_t"
#define _KEY_HEATER_HIGH_TEMP "h_high_t"



using namespace fb;
using namespace settings;



void settings::setLightStartTime(clock::Timestamp t)
{
	settings::setInt(_PARTITION, _KEY_LIGHT_START, t);
}

void settings::setLightEndTime(clock::Timestamp t)
{
	settings::setInt(_PARTITION, _KEY_LIGHT_END, t);
}

void settings::setFanSpeed(int speed)
{
	settings::setInt(_PARTITION, _KEY_FAN_SPEED, speed);
}

void settings::setFanLowTemp(float value)
{
	char tmp[16] = {0};
	sprintf(tmp, "%.6f", value);

	settings::setStr(_PARTITION, _KEY_FAN_LOW_TEMP, tmp);
}

void settings::setFanHighTemp(float value)
{
	char tmp[16] = {0};
	sprintf(tmp, "%.6f", value);

	settings::setStr(_PARTITION, _KEY_FAN_HIGH_TEMP, tmp);
}

void settings::setHeaterLowTemp(float value)
{
	char tmp[16] = {0};
	sprintf(tmp, "%.6f", value);

	settings::setStr(_PARTITION, _KEY_HEATER_LOW_TEMP, tmp);
}

void settings::setHeaterHighTemp(float value)
{
	char tmp[16] = {0};
	sprintf(tmp, "%.6f", value);

	settings::setStr(_PARTITION, _KEY_HEATER_HIGH_TEMP, tmp);
}



clock::Timestamp settings::getLightStartTime()
{
	return settings::getIntOrDefault(_PARTITION, _KEY_LIGHT_START, 0);
}

clock::Timestamp settings::getLightEndTime()
{
	return settings::getIntOrDefault(_PARTITION, _KEY_LIGHT_END, 0);
}

int settings::getFanSpeed()
{
	return settings::getIntOrDefault(_PARTITION, _KEY_FAN_SPEED, 100);
}

float settings::getFanLowTemp()
{
	auto val = settings::getStrOrDefault(_PARTITION, _KEY_FAN_LOW_TEMP, "27.0");
	return std::atof(val.c_str());
}

float settings::getFanHighTemp()
{
	auto val = settings::getStrOrDefault(_PARTITION, _KEY_FAN_HIGH_TEMP, "28.0");
	return std::atof(val.c_str());
}

float settings::getHeaterLowTemp()
{
	auto val = settings::getStrOrDefault(_PARTITION, _KEY_HEATER_LOW_TEMP, "23.0");
	return std::atof(val.c_str());
}

float settings::getHeaterHighTemp()
{
	auto val = settings::getStrOrDefault(_PARTITION, _KEY_HEATER_HIGH_TEMP, "27.0");
	return std::atof(val.c_str());
}
