#include "fb_globals.hpp"

#include <memory>



using namespace fb;
using namespace global;



static std::unique_ptr<event::EventManager> _eventManager;
static std::unique_ptr<pins::PinManager> _pinManager;
static std::unique_ptr<state::StateManager> _stateManager;



void global::init()
{
	assert(!_eventManager);
	_eventManager = std::make_unique<event::EventManager>();
	_pinManager = std::make_unique<pins::PinManager>();
	_stateManager = std::make_unique<state::StateManager>(("MainStateManager"));
}

event::EventManager* global::getEventManager()
{
	return _eventManager.get();
}

pins::PinManager* global::getPinManager()
{
	return _pinManager.get();
}

state::StateManager* global::getStateManager()
{
	return _stateManager.get();
}