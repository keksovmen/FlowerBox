#include "fb_core.hpp"

#include "fb_globals.hpp"
#include "fb_input_matrix_box_obj.hpp"
#include "fb_input_matrix_hw_obj.hpp"
#include "fb_input_matrix_mapper_obj.hpp"



using namespace fb;
using namespace project;



static void _handleEvent(const event::Event& event);
 


static BaseCore _core{&_handleEvent, "InputMatrix"};

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