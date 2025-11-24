#include "fb_core.hpp"

#include "fb_server_html.hpp"



using namespace fb;
using namespace project;



static BaseCore _core{{}, "Relay"};



static CoreInfo _info = {
	.core = _core,
	.requiresTime = false,
};



void project::bootInit()
{
}

const CoreInfo& project::getInfo()
{
	return _info;
}