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
	FB_DEBUG_ENTER();

	//TODO: put mutex because on transition call will change ptr
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

	//TODO: put mutex because on transition call will change ptr

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

	if(_currentState){
		FB_DEBUG_LOG("%s->exit()", _currentState->getName());
		_currentState->exit();
		_currentState.reset(nullptr);
	}
	
	FB_DEBUG_EXIT();
}




CompositeState::CompositeState(StateManager& context, const std::string& name)
	: State(context), StateManager(name)
{

}

void CompositeState::enter()
{
	FB_DEBUG_ENTER();

	_onCompositeEnter();
	init(_createInitialState());

	FB_DEBUG_EXIT();
}

void CompositeState::exit()
{
	FB_DEBUG_ENTER();

	_onCompositeExit();
	deinit();

	FB_DEBUG_EXIT();
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
