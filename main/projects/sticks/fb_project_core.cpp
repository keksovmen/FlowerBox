#include "fb_core.hpp"



using namespace fb;
using namespace project;



static BaseCore _core{{}, ""};



static CoreInfo _info = {
	.core = _core,
	.requiresTime = false,
	.requiresServices = false,
};



void project::bootInit()
{
}

const CoreInfo& project::getInfo()
{
	return _info;
}