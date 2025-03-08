#pragma once



#include "fb_debug.hpp"
#include "fb_event_manager.hpp"
#include "h_buttons.hpp"



namespace fb
{
	namespace keyboard
	{
		class KeyboardHandler : public debug::Named, public event::EventListener
		{
			public:

				virtual const char* getName() const override;
				virtual void handleEvent(const event::Event& event) override;
			
			protected:
				virtual void _handleButtonAction(const h::ButtonAction& action);
		};
	}
}