#pragma once



#include "fb_state_manager.hpp"



namespace fb
{
	namespace state
	{
		class StateBooted : public State
		{
			public:
				StateBooted(StateManager& context);

				virtual const char* getName() override;

				virtual void handleEvent(const event::Event& event) override;

				virtual void enter() override;
				virtual void exit() override;
		};
	}
}