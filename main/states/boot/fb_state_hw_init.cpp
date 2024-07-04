#include "fb_state_hw_init.hpp"

#include "fb_file_system.hpp"
#include "fb_state_wifi_init.hpp"



using namespace fb;
using namespace state;



StateHwInit::StateHwInit(StateManager& context)
	: State(context)
{

}

const char* StateHwInit::getName()
{
	return "StateHwInit";
}

void StateHwInit::handleEvent(const event::Event& event)
{

}

void StateHwInit::enter()
{
	fs::init();
	
	getContext().transition(std::make_unique<StateWifiInit>(getContext()));
}

void StateHwInit::exit()
{

}
