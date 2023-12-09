#include "fb_state_wifi_connected.hpp"

#include "fb_http_server.hpp"
#include "fb_state_time_init.hpp"
#include "fb_state_wifi_connect.hpp"
#include "fb_wifi.hpp"



using namespace fb;
using namespace state;




StateWifiConnected::StateWifiConnected(StateManager& context)
	: State(context), _childManager("StateWifiConnectedManager")
{

}

const char* StateWifiConnected::getName()
{
	return "StateWifiConnected";
}

void StateWifiConnected::handleEvent(const event::Event& event)
{
	if(event.groupId == event::EventGroup::WIFI){
		if(event.eventId == wifi::WifiEventId::DISCONNECTED){
			getContext().transition(std::make_unique<StateWifiConnect>(getContext()));
			return;
		}
	}

	_childManager.handleEvent(event);
}

void StateWifiConnected::enter()
{
	_childManager.init(std::make_unique<StateTimeInit>(_childManager));
	http::startServer();
}

void StateWifiConnected::exit()
{
	_childManager.deinit();
	http::stopServer();
}
