#include "fb_state_initialized.hpp"

#include "fb_globals.hpp"



using namespace fb;
using namespace state;



StateInitialized::StateInitialized(StateManager& context)
	: State(context)
{
	
}

const char* StateInitialized::getName()
{
	return "StateInitialized";
}

void StateInitialized::enter()
{
	global::getEventManager()->attachListener(global::getPinManager());
}

void StateInitialized::exit()
{
	global::getEventManager()->detachListener(global::getPinManager());
}
