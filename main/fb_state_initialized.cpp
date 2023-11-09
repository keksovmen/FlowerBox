#include "fb_state_initialized.hpp"



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

}

void StateInitialized::exit()
{

}
