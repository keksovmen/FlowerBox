#include "fb_keypad_backlight_box_obj.hpp"

#include "fb_globals.hpp"



#define _DEVICE_NAME "KeypadBacklight"
#define _DEVICE_VERSION "0.0.1"



using namespace fb;
using namespace project;



static box::Box _flowerBox(_DEVICE_NAME, _DEVICE_VERSION, global::getUniqueId());



void project::initBoxObjs()
{

}

box::Box& project::getBox()
{
	return _flowerBox;	
}