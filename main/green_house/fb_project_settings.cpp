#include "fb_green_house_settings.hpp"

#include <cstring>
#include <cstdlib>

#include "fb_settings.hpp"



#define _PARTITION "prj_set"

#define _KEY_LIGHT_START "l_on"
#define _KEY_LIGHT_END "l_off"

#define _KEY_FAN_SPEED "f_speed"
#define _KEY_FAN_LOW_TEMP "f_low_t"
#define _KEY_FAN_HIGH_TEMP "f_high_t"
#define _KEY_FAN_LOW_HUMIDITY "f_low_h"
#define _KEY_FAN_HIGH_HUMIDITY "f_high_h"
#define _KEY_FAN_DAY_NIGHT_DELTA_HUMIDITY "f_delta"

#define _KEY_HEATER_LOW_TEMP "h_low_t"
#define _KEY_HEATER_HIGH_TEMP "h_high_t"
#define _KEY_HEATER_DAY_NIGHT_DELTA_TEMP "h_delta"



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
	settings::setFloat(_PARTITION, _KEY_FAN_LOW_TEMP, value);
}

void settings::setFanHighTemp(float value)
{
	settings::setFloat(_PARTITION, _KEY_FAN_HIGH_TEMP, value);
}

void settings::setFanLowHum(float value)
{
	settings::setFloat(_PARTITION, _KEY_FAN_LOW_HUMIDITY, value);
}

void settings::setFanHighHum(float value)
{
	settings::setFloat(_PARTITION, _KEY_FAN_HIGH_HUMIDITY, value);
}

void settings::setFanDayNightDelta(float value)
{
	settings::setFloat(_PARTITION, _KEY_FAN_DAY_NIGHT_DELTA_HUMIDITY, value);
}

void settings::setHeaterLowTemp(float value)
{
	settings::setFloat(_PARTITION, _KEY_HEATER_LOW_TEMP, value);
}

void settings::setHeaterHighTemp(float value)
{
	settings::setFloat(_PARTITION, _KEY_HEATER_HIGH_TEMP, value);
}

void settings::setHeaterDayNightDelta(float value)
{
	settings::setFloat(_PARTITION, _KEY_HEATER_DAY_NIGHT_DELTA_TEMP, value);
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
	return settings::getFloatOrDefault(_PARTITION, _KEY_FAN_LOW_TEMP, 25.0f);
}

float settings::getFanHighTemp()
{
	return settings::getFloatOrDefault(_PARTITION, _KEY_FAN_HIGH_TEMP, 26.0f);
}

float settings::getFanLowHum()
{
	return settings::getFloatOrDefault(_PARTITION, _KEY_FAN_LOW_HUMIDITY, 60.0f);
}

float settings::getFanHighHum()
{
	return settings::getFloatOrDefault(_PARTITION, _KEY_FAN_HIGH_HUMIDITY, 70.0f);
}

float settings::getFanDayNightDelta()
{
	return settings::getFloatOrDefault(_PARTITION, _KEY_FAN_DAY_NIGHT_DELTA_HUMIDITY, -10.0f);
}

float settings::getHeaterLowTemp()
{
	return settings::getFloatOrDefault(_PARTITION, _KEY_HEATER_LOW_TEMP, 23.0f);
}

float settings::getHeaterHighTemp()
{
	return settings::getFloatOrDefault(_PARTITION, _KEY_HEATER_HIGH_TEMP, 25.0f);
}

float settings::getHeaterDayNightDelta()
{
	return settings::getFloatOrDefault(_PARTITION, _KEY_HEATER_DAY_NIGHT_DELTA_TEMP, 4.0f);
}