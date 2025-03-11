#include "fb_project_settings.hpp"

#include "fb_settings.hpp"



#define _PARTITION "prj_set"

#define _KEY_LIGHT_START "l_on"
#define _KEY_LIGHT_END "l_off"
#define _KEY_FAN_SPEED "f_speed"



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

