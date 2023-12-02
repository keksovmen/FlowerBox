#include "fb_state_time_init.hpp"

#include "fb_clock.hpp"
#include "fb_globals.hpp"



using namespace fb;
using namespace state;



StateTimeInit::StateTimeInit(StateManager& context)
	: State(context)
{
	
}

const char* StateTimeInit::getName()
{
	return "StateTimeInit";
}

void StateTimeInit::handleEvent(const event::Event& event)
{
	if(event.groupId == event::EventGroup::CLOCK){
		if(event.eventId == clock::ClockEventId::SYNCED){
			//transition to working state
			FB_DEBUG_LOG("Time synced, ready to work");
		}
	}
}

void StateTimeInit::enter()
{
	clock::syncRequest();
}

void StateTimeInit::exit()
{

}
