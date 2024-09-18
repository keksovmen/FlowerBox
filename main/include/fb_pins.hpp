#pragma once



#include "fb_event_manager.hpp"
#include "fb_debug.hpp"



namespace fb
{
	namespace pins
	{
		static constexpr int PIN_SENSOR_TEMPERATURE = 10;
		static constexpr int PIN_GREEN_LED = 4;
		static constexpr int PIN_BLUE_LED = 5;
		// static constexpr int PIN_SENSOR_TEMPERATURE = 10;


		class PinManager : public debug::Named, public event::EventListener
		{
			public:
				virtual const char* getName() override;
				virtual void handleEvent(const event::Event& event) override;
			
			private:
				void _onBootEvent(const event::Event& event);
		};

	}
}