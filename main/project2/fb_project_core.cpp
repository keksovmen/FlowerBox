#include "fb_project_core.hpp"

#include "fb_server_html.hpp"
#include "fb_project_maper_obj.hpp"



using namespace fb;
using namespace project;



static void _controlsCb(templates::Engine& engine)
{
	engine.addIntArg(getRgbProperty().getId(), "rgb_color_id");
	engine.addIntArg(getRgbSwitchProperty().getId(), "rgb_switch_id");

	engine.addIntArg(getMp3PlayProperty().getId(), "play_id");
	engine.addIntArg(getMp3StopProperty().getId(), "stop_id");
	engine.addIntArg(getMp3ValueProperty().getId(), "volume_id");

	engine.addIntArg(std::stoi(getMp3ValueProperty().getValue()), "volume_val");
}



void project::init()
{
	server::htmlAddFileHandler("controls.html", &_controlsCb);	
}