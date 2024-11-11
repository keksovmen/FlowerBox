#pragma once



#include "fb_state_manager.hpp"



namespace fb
{
	namespace state
	{
		class StateWifiConnect : public State
		{
			public:
				StateWifiConnect(StateManager& context);

				virtual const char* getName() const override;

				virtual void handleEvent(const event::Event& event);

				virtual void enter() override;
				virtual void exit() override;
		};
	}
}