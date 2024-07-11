#pragma once



#include "fb_box.hpp"
#include "fb_event_manager.hpp"
#include "fb_id.hpp"
#include "fb_pins.hpp"
#include "fb_state_manager.hpp"



namespace fb
{
	namespace global
	{
		void init();



		event::EventManager* getEventManager();
		pins::PinManager* getPinManager();
		state::StateManager* getStateManager();

		id::UniqueId getUniqueId();
		box::Box* getFlowerBox();
	}
}