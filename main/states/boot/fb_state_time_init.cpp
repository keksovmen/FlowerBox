#include "fb_state_time_init.hpp"

#include "fb_clock.hpp"
#include "fb_globals.hpp"
#include "fb_state_wifi_init.hpp"
#include "fb_wifi.hpp"



using namespace fb;
using namespace state;



StateTimeInit::StateTimeInit(StateManager& context)
	: State(context)
{

}

const char* StateTimeInit::getName() const
{
	return "StateTimeInit";
}

void StateTimeInit::handleEvent(const event::Event& event)
{
	if(event.groupId == event::EventGroup::CLOCK){
		if(event.eventId == clock::ClockEventId::SYNCED){
			//transition to working state
			FB_DEBUG_LOG("Time synced, ready to work");
			global::getEventManager()->pushEvent(event::Event{fb::event::EventGroup::TRANSITION, 0, nullptr});
		}
	}else if(event.groupId == event::EventGroup::WIFI){
		//handle on wifi connect and on wifi failure events
		if(event.eventId == wifi::WifiEventId::DISCONNECTED){
			getContext().transition(std::make_unique<StateWifiInit>(getContext()));
		}
	}
}

void StateTimeInit::enter()
{
	// clock::initClock();
	// clock::syncRequest();

	global::getEventManager()->pushEvent(event::Event{fb::event::EventGroup::TRANSITION, 0, nullptr});


	//TODO: start timer if no clock event is provided then something wrong with internet access
}

void StateTimeInit::exit()
{
	clock::deinitClock();
}
