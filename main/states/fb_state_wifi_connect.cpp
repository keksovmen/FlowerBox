#include "fb_state_wifi_connect.hpp"

#include "fb_clock.hpp"
#include "fb_state_wifi_connected.hpp"
#include "fb_wifi.hpp"



using namespace fb;
using namespace state;



StateWifiConnect::StateWifiConnect(StateManager& context)
	: State(context)
{

}

const char* StateWifiConnect::getName() const
{
	return "StateWifiConnect";
}

void StateWifiConnect::handleEvent(const event::Event& event)
{
	if(event.groupId == event::EventGroup::WIFI){
		//handle on wifi connect and on wifi failure events
		if(event.eventId == wifi::WifiEventId::CONNECTED){
			getContext().transition(std::make_unique<StateWifiConnected>(getContext()));

		}else if(event.eventId == wifi::WifiEventId::FAILED_TO_CONNECT){
			//go to provision state
		}
	}
}

void StateWifiConnect::enter()
{
	//TODO: obtain from NVS
	wifi::connect(CONFIG_EXAMPLE_WIFI_SSID, CONFIG_EXAMPLE_WIFI_PASSWORD);
}

void StateWifiConnect::exit()
{
	//все оставляем как есть
}
