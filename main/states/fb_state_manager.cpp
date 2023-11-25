#include "fb_state_manager.hpp"

#include <utility>



using namespace fb;
using namespace state;



State::State(StateManager& context)
	: _context(context)
{

}

State::~State()
{

}

StateManager& State::getContext()
{
	return _context;
}



StateManager::StateManager(const std::string& name)
	: _name(name)
{

}

const char* StateManager::getName()
{
	return _name.c_str();
}

void StateManager::handleEvent(const event::Event& event)
{
	FB_DEBUG_ENTER();

	FB_DEBUG_LOG("%s->handleEvent()", _currentState->getName());
	_currentState->handleEvent(event);
	
	FB_DEBUG_EXIT();
}

void StateManager::init(std::unique_ptr<State>&& initialState)
{
	FB_DEBUG_ENTER();

	assert(initialState);
	_currentState = std::move(initialState);

	FB_DEBUG_LOG("%s->enter()", _currentState->getName());
	_currentState->enter();

	FB_DEBUG_EXIT();
}

void StateManager::transition(std::unique_ptr<State>&& nextState)
{
	FB_DEBUG_ENTER();

	assert(_currentState);
	assert(nextState);

	FB_DEBUG_LOG("%s->exit()", _currentState->getName());
	_currentState->exit();

	_currentState = std::move(nextState);

	FB_DEBUG_LOG("%s->enter()", _currentState->getName());
	_currentState->enter();

	FB_DEBUG_EXIT();
}

void StateManager::deinit()
{
	FB_DEBUG_ENTER();

	assert(_currentState);

	FB_DEBUG_LOG("%s->exit()", _currentState->getName());
	_currentState->exit();

	FB_DEBUG_EXIT();
}
