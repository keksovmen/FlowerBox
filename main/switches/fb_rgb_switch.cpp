#include "fb_rgb_switch.hpp"



using namespace fb;
using namespace switches;



RgbSwitch::RgbSwitch(int gpioR, int gpioG, int gpioB)
	: SwitchIface(&_condition, &_action)
{

}

const char* RgbSwitch::getName() const
{
	return "RgbSwitch";
}

void RgbSwitch::setColor(int color)
{

}

int RgbSwitch::getColor() const
{
	return 0;
}

bool RgbSwitch::_condition(SwitchIface* me)
{
	return false;
}

void RgbSwitch::_action(SwitchIface* me, bool value)
{

}
