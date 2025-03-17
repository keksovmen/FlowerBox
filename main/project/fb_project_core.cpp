#include "fb_project_core.hpp"

#include "fb_globals.hpp"
#include "fb_project_hw_obj.hpp"



using namespace fb;
using namespace project;



static CoreInfo _info = {
	.requiresTime = true,
};


void project::init()
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