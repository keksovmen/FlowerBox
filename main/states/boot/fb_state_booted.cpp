#include "fb_state_booted.hpp"

#include "fb_state_hw_init.hpp"
#include "fb_state_work.hpp"



using namespace fb;
using namespace state;



StateBooted::StateBooted(StateManager& context)
	: CompositeState(context, "StateBooted")
{

}

bool StateBooted::_onCompositeEventHandler(const event::Event& event)
{
	if(event.groupId == event::EventGroup::TRANSITION){
		getContext().transition(std::make_unique<StateWork>(getContext()));
		return false;
	}

	return true;
}

std::unique_ptr<State> StateBooted::_createInitialState()
{
	return std::make_unique<StateHwInit>(*this);
}
