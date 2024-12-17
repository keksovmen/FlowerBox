#include "fb_mp3_switch.hpp"

#include <mutex>
#include "serial.h"
#include "DFRobotDFPlayerMini.h"



using namespace fb;
using namespace switches;



static std::once_flag _flag;



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
	std::call_once(_flag, [](){
		serial_begin(9600, 8, 9);
	});

	//write uart cmd
	//set forse flag
	setForseFlag(SwitchForseState::ON);
	DF_play(0);
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
