#include "fb_core.hpp"

#include "fb_globals.hpp"
#include "fb_keypad_backlight_box_obj.hpp"
#include "fb_keypad_backlight_hw_obj.hpp"
#include "fb_keypad_backlight_mapper_obj.hpp"



using namespace fb;
using namespace project;



static void _handleEvent(const event::Event& event);
 


static BaseCore _core{&_handleEvent, "KeypadBacklight"};

static CoreInfo _info = {
	.core = _core,
	.requiresTime = false,
	.requireSensorService = true,
	.requireSwitchService = false,
};



static void _handleEvent(const event::Event& event)
{
	//do stuff
}



void project::bootInit()
{

}

const CoreInfo& project::getInfo()
{
	return _info;
}