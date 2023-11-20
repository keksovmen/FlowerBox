#pragma once



#include "fb_event_manager.hpp"
#include "fb_debug.hpp"



namespace fb
{
	namespace pins
	{
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