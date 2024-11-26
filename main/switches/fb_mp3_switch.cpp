#include "fb_mp3_switch.hpp"



using namespace fb;
using namespace switches;



Mp3Switch::Mp3Switch()
	: SwitchIface(&_condition, &_action)
{

}

const char* Mp3Switch::getName() const
{
	return "Mp3Switch";
}

bool Mp3Switch::play(int trackId)
{
	//write uart cmd
	//set forse flag
	setForseFlag(SwitchForseState::ON);
	return true;
}

void Mp3Switch::stop()
{
	setForseFlag(SwitchForseState::OFF);
}

int Mp3Switch::getTrack() const
{
	return 0;
}

bool Mp3Switch::_condition(SwitchIface* me)
{
	return false;
}

void Mp3Switch::_action(SwitchIface* me, bool value)
{

}
