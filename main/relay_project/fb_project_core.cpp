#include "fb_project_core.hpp"

#include "fb_server_html.hpp"
#include "fb_project_maper_obj.hpp"



using namespace fb;
using namespace project;



static CoreInfo _info = {
	.requiresTime = false,
};



void project::bootInit()
{
}

const CoreInfo& project::getInfo()
{
	return _info;
}