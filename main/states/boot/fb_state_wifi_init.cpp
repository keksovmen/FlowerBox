#include "fb_state_wifi_init.hpp"

#include "fb_state_time_init.hpp"
#include "fb_wifi.hpp"



using namespace fb;
using namespace state;



StateWifiInit::StateWifiInit(StateManager& context)
	: State(context)
{

}

const char* StateWifiInit::getName()
{
	return "StateWifiInit";
}

void StateWifiInit::handleEvent(const event::Event& event)
{
	if(event.groupId == event::EventGroup::WIFI){
		//handle on wifi connect and on wifi failure events
		if(event.eventId == wifi::WifiEventId::CONNECTED){
			getContext().transition(std::make_unique<StateTimeInit>(getContext()));

		}else if(event.eventId == wifi::WifiEventId::FAILED_TO_CONNECT){
			//TODO:
			//go to provision state
		}
	}
}

void StateWifiInit::enter()
{
	wifi::init();
	//TODO: add logic for wifi mode AP or STA
	wifi::connect(CONFIG_EXAMPLE_WIFI_SSID, CONFIG_EXAMPLE_WIFI_PASSWORD);
}

void StateWifiInit::exit()
{

}
