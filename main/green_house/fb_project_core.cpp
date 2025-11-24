#include "fb_core.hpp"

#include "fb_globals.hpp"
#include "fb_green_house_hw_obj.hpp"



using namespace fb;
using namespace project;



static void _handleEvent(const event::Event& event);
 



static BaseCore _core{&_handleEvent, "GreenHouse"};

static CoreInfo _info = {
	.core = _core,
	.requiresTime = true,
};



static void _handleEvent(const event::Event& event)
{
	//do stuff
}



void project::bootInit()
{
	global::getSensorStoreService()->registerSensor({
		&getHwAhtSensor(),
		[](int index){return index == 0 ? 0.5f : 2.0f;}
	});
	global::getSensorStoreService()->registerSensor({
		&getHwDS18Sensor(),
		[](int index){return 0.3f;}
	});
}

const CoreInfo& project::getInfo()
{
	return _info;
}