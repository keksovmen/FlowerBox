#pragma once



#include <string>

#include "fb_box.hpp"
#include "fb_event_manager.hpp"
#include "fb_id.hpp"
#include "fb_sensor_service.hpp"
#include "fb_sensor_storage.hpp"
#include "fb_sensor_store_service.hpp"
#include "fb_state_manager.hpp"
#include "fb_switch_iface.hpp"
#include "fb_switch_service.hpp"
#include "fb_time_scheduler.hpp"



namespace fb
{
	namespace global
	{
		void init();



		event::EventManager* getEventManager();
		state::StateManager* getStateManager();

		id::UniqueId getUniqueId();
		box::Box* getFlowerBox();

		sensor::SensorService* getSensorService();
		sensor::SensorStorage* getSensorStorage();
		sensor::SensorStoreService* getSensorStoreService();

		switches::SwitchService* getSwitchService();

		std::string getDeviceName();

		util::TimeScheduler* getTimeScheduler();
	}
}