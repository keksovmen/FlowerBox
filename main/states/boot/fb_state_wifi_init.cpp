#include "fb_state_wifi_init.hpp"

#include "fb_core.hpp"
#include "fb_globals.hpp"
#include "fb_settings.hpp"
#include "fb_state_time_init.hpp"
#include "fb_wifi.hpp"


using namespace fb;
using namespace state;



StateWifiInit::StateWifiInit(StateManager& context)
	: State(context)
{

}

const char* StateWifiInit::getName() const
{
	return "StateWifiInit";
}

void StateWifiInit::handleEvent(const event::Event& event)
{
	if(event.groupId == event::EventGroup::WIFI){
		//handle on wifi connect and on wifi failure events
		if(event.eventId == wifi::WifiEventId::CONNECTED){
			//conditional time state
			if(project::getInfo().requiresTime){
				getContext().transition(std::make_unique<StateTimeInit>(getContext()));
			}else{
				global::getEventManager()->pushEvent(event::Event{fb::event::EventGroup::TRANSITION, 0, nullptr});
			}

		}else if(event.eventId == wifi::WifiEventId::FAILED_TO_CONNECT){
			FB_DEBUG_LOG_I_OBJ("Failed to connect, retrying");
			wifi::reconnect();
			// abort();
			//TODO:
			//go to provision state
			//or try again until network is provided
		}else if(event.eventId == wifi::WifiEventId::AP_STARTED){
			//we are working as AP so go to next state ignoring time init???
			//for now let say at AP we do not have init clock source
			global::getEventManager()->pushEvent(event::Event{fb::event::EventGroup::TRANSITION, 0, nullptr});
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
