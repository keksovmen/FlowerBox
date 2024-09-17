#pragma once



#include "fb_box.hpp"
#include "fb_event_manager.hpp"
#include "fb_id.hpp"
#include "fb_pins.hpp"
#include "fb_sensor_service.hpp"
#include "fb_state_manager.hpp"
#include "fb_sensor_storage.hpp"
#include "fb_switch_service.hpp"

#include "fb_switch_iface.hpp"



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

		sensor::SensorService* getSensorService();
		sensor::SensorStorage* getSensorStorage();

		switches::SwitchService* getSwitchService();
	}
}