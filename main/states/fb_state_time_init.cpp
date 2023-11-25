#include "fb_state_time_init.hpp"

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

}

void StateTimeInit::enter()
{
	// global::getEventManager()->attachListener(global::getPinManager());
}

void StateTimeInit::exit()
{
	// global::getEventManager()->detachListener(global::getPinManager());
}
