#pragma once



#include "fb_event_manager.hpp"
#include "fb_state_manager.hpp"
#include "fb_pins.hpp"


namespace fb
{
	namespace global
	{
		void init();

		event::EventManager* getEventManager();
		pins::PinManager* getPinManager();
		state::StateManager* getStateManager();
	}
}