#include "fb_state_work.hpp"

#include "fb_globals.hpp"
#include "fb_server.hpp"
#include "fb_state_wifi_connected.hpp"
#include "fb_update.hpp"

#include "esp_system.h"



using namespace fb;
using namespace state;



StateWork::StateWork(StateManager& context)
	: CompositeState(context, "StateWork")
{

}

void StateWork::_onCompositeEnter()
{
	server::startWorking();
	// http::startServer();
}

void StateWork::_onCompositeExit()
{
	server::stop();
	// http::stopServer();
}

bool StateWork::_onCompositeEventHandler(const event::Event& e)
{
	if(e.groupId == event::EventGroup::UPDATE){
		if(e.eventId == update::UpdateEventId::END){
			//restart the system, but not right now, but with a delay, so http can send response
			global::restart();
		}

		// return false;
	}

	return true;
}

std::unique_ptr<State> StateWork::_createInitialState()
{
	return std::make_unique<StateWifiConnected>(*this);
}