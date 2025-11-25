#include "fb_core.hpp"

#include "fb_globals.hpp"
#include "fb_tressure_box_obj.hpp"
#include "fb_tressure_hw_obj.hpp"
#include "fb_tressure_mapper_obj.hpp"



using namespace fb;
using namespace project;



static void _handleEvent(const event::Event& event);
 


static BaseCore _core{&_handleEvent, ""};

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

}

const CoreInfo& project::getInfo()
{
	return _info;
}