#include "fb_state_work.hpp"

// #include "fb_http_server.hpp"
#include "fb_state_wifi_connected.hpp"
#include "fb_server.hpp"



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

bool StateWork::_onCompositeEventHandler(const event::Event& event)
{
	return true;
}

std::unique_ptr<State> StateWork::_createInitialState()
{
	return std::make_unique<StateWifiConnected>(*this);
}