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

void StateManager::init(std::unique_ptr<State>&& initialState)
{
	FB_DEBUG_ENTER();

	assert(initialState);
	_currentState = std::move(initialState);

	ESP_LOGI(getName(), "%s->enter()", _currentState->getName());
	_currentState->enter();

	FB_DEBUG_EXIT();
}

void StateManager::transition(std::unique_ptr<State>&& nextState)
{
	FB_DEBUG_ENTER();

	assert(_currentState);
	assert(nextState);

	ESP_LOGI(getName(), "%s->exit()", _currentState->getName());
	_currentState->exit();

	_currentState = std::move(nextState);

	ESP_LOGI(getName(), "%s->enter()", _currentState->getName());
	_currentState->enter();

	FB_DEBUG_EXIT();
}

void StateManager::deinit()
{
	FB_DEBUG_ENTER();

	assert(_currentState);

	ESP_LOGI(getName(), "%s->exit()", _currentState->getName());
	_currentState->exit();

	FB_DEBUG_EXIT();
}
