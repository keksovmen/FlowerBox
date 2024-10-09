#include "fb_state_wifi_init.hpp"

#include "fb_state_time_init.hpp"
#include "fb_settings.hpp"
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
			FB_DEBUG_LOG("Failed to connect");
			abort();
			//TODO:
			//go to provision state
			//or try again until network is provided
		}
	}
}

void StateWifiInit::enter()
{
	assert(wifi::start(
		wifi::WifiConfig{
			settings::getApSsid(),
			settings::getApPass(),
			settings::getStaSsid(),
			settings::getStaPass(),
			settings::getWifiMode() == settings::WifiMode::STA ?
				wifi::WifiState::STA : wifi::WifiState::AP}
	));
}

void StateWifiInit::exit()
{

}
