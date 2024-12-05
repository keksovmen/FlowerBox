#include "fb_state_manager.hpp"

#include <utility>



using namespace fb;
using namespace state;



State::State(StateManager& context)
	: _context(context)
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

StateManager::~StateManager()
{
	deinit();
}

const char* StateManager::getName() const
{
	return _name.c_str();
}

void StateManager::handleEvent(const event::Event& event)
{
	FB_DEBUG_ENTER_I_OBJ();

	//TODO: put mutex because on transition call will change ptr
	FB_DEBUG_LOG_I_OBJ("%s->handleEvent()", _currentState->getName());
	_currentState->handleEvent(event);
}

void StateManager::init(std::unique_ptr<State>&& initialState)
{
	FB_DEBUG_ENTER_I_OBJ();

	assert(initialState);
	_currentState = std::move(initialState);

	FB_DEBUG_LOG_I_OBJ("%s->enter()", _currentState->getName());
	_currentState->enter();
}

void StateManager::transition(std::unique_ptr<State>&& nextState)
{
	FB_DEBUG_ENTER_I_OBJ();

	assert(_currentState);
	assert(nextState);

	//TODO: put mutex because on transition call will change ptr

	FB_DEBUG_LOG_I_OBJ("%s->exit()", _currentState->getName());
	_currentState->exit();

	_currentState = std::move(nextState);

	FB_DEBUG_LOG_I_OBJ("%s->enter()", _currentState->getName());
	_currentState->enter();
}

void StateManager::deinit()
{
	FB_DEBUG_ENTER_I_OBJ();

	if(_currentState){
		FB_DEBUG_LOG_I_OBJ("%s->exit()", _currentState->getName());
		_currentState->exit();
		_currentState.reset(nullptr);
	}
}




CompositeState::CompositeState(StateManager& context, const std::string& name)
	: State(context), StateManager(name)
{

}

void CompositeState::enter()
{
	FB_DEBUG_ENTER_I_OBJ();

	_onCompositeEnter();
	init(_createInitialState());
}

void CompositeState::exit()
{
	FB_DEBUG_ENTER_I_OBJ();

	_onCompositeExit();
	deinit();
}

void CompositeState::handleEvent(const event::Event& event)
{
	if(_onCompositeEventHandler(event)){
		StateManager::handleEvent(event);
	}
}


void CompositeState::_onCompositeEnter()
{

}

void CompositeState::_onCompositeExit()
{

}

bool CompositeState::_onCompositeEventHandler(const event::Event& event)
{
	return true;
}
