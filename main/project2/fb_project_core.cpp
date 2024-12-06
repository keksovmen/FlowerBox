#include "fb_project_core.hpp"

#include "fb_server_html.hpp"
#include "fb_project_maper_obj.hpp"



using namespace fb;
using namespace project;



static void _controlsCb(templates::Engine& engine)
{
	engine.addIntArg(getRgbProperty().getId(), "color_id");
	engine.addIntArg(getRgbSwitchProperty().getId(), "switch_id");
}



void project::init()
{
	server::htmlAddFileHandler("controls.html", &_controlsCb);	
}