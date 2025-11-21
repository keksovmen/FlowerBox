#include "fb_state_wifi_connected.hpp"

#include "fb_state_time_init.hpp"
#include "fb_state_wifi_connect.hpp"
#include "fb_wifi.hpp"



using namespace fb;
using namespace state;




StateWifiConnected::StateWifiConnected(StateManager& context)
	: State(context)
{

}

const char* StateWifiConnected::getName() const
{
	return "StateWifiConnected";
}

void StateWifiConnected::handleEvent(const event::Event& event)
{
	if(event.groupId == event::EventGroup::WIFI){
		if(event.eventId == wifi::WifiEventId::FAILED_TO_CONNECT){
			// getContext().transition(std::make_unique<StateWifiConnect>(getContext()));
			// return;
			//TODO: made proper state for no wifi and reconnect in there
			wifi::reconnect();
		}
	}

	// _childManager.handleEvent(event);
}

void StateWifiConnected::enter()
{
	// _childManager.init(std::make_unique<StateTimeInit>(_childManager));
	// http::startServer();
}

void StateWifiConnected::exit()
{
	// _childManager.deinit();
	// http::stopServer();
}
