#pragma once



#include "fb_state_manager.hpp"




namespace fb
{
	namespace state
	{
		class StateWifiConnected : public State
		{
			public:
				StateWifiConnected(StateManager& context);

				virtual const char* getName() override;

				virtual void handleEvent(const event::Event& event);

				virtual void enter() override;
				virtual void exit() override;

			private:
				StateManager _childManager;
		};
	}
}