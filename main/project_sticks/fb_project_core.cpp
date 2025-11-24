#include "fb_project_core.hpp"



using namespace fb;
using namespace project;



static CoreInfo _info = {
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