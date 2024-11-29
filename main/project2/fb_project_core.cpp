#include "fb_project_core.hpp"

#include "fb_server_html.hpp"



using namespace fb;
using namespace project;



void _controlsCb(templates::Engine& engine)
{
	engine.addIntArg(228, "LOH");
}



void project::init()
{
	server::htmlAddFileHandler("controls.html", &_controlsCb);	
}