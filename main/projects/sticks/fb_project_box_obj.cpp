#include "fb_core.hpp"

#include "fb_globals.hpp"
#include "fb_property.hpp"



#define _DEVICE_NAME "Sticks controller"
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