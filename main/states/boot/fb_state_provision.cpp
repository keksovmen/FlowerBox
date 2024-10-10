#include "fb_state_provision.hpp"

#include "fb_state_wifi_init.hpp"
#include "fb_provision.hpp"



#define _ENDPOINT_WIFI_MODE "wifi_mode"



using namespace fb;
using namespace state;



StateProvision::StateProvision(StateManager& context)
	: State(context)
{

}

const char* StateProvision::getName()
{
	return "StateProvision";
}

void StateProvision::handleEvent(const event::Event& event)
{
	if(event.groupId == event::EventGroup::PROVISION){
		if(event.eventId == provision::ProvisionEventId::SUCCESS){
			getContext().transition(std::make_unique<StateWifiInit>(getContext()));
		}
	}
}

void StateProvision::enter()
{
	provision::init();
}

void StateProvision::exit()
{
	provision::deinit();
}
