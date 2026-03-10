#include "fb_core.hpp"

#include "fb_globals.hpp"
#include "fb_rfids_box_obj.hpp"
#include "fb_rfids_hw_obj.hpp"
#include "fb_rfids_mapper_obj.hpp"



using namespace fb;
using namespace project;



static void _handleEvent(const event::Event& event);
 


static BaseCore _core{&_handleEvent, "RfidArray"};

static CoreInfo _info = {
	.core = _core,
	.requiresTime = false,
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