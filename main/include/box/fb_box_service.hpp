#pragma once



#include "fb_box.hpp"
#include "fb_debug.hpp"
#include "fb_event_manager.hpp"
#include "fb_sensor_storage.hpp"



namespace fb
{
	namespace box
	{
		class BoxService : public event::EventListener, public debug::Named
		{
			public:
				BoxService(Box& box, sensor::SensorStorage& storage);

				virtual void handleEvent(const event::Event& event) override;
				virtual const char* getName() override;
			
			private:
				Box& _box;
				sensor::SensorStorage& _storage;
				
		};
	} // namespace box
	
}