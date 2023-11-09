#pragma once



#include "fb_state_manager.hpp"



namespace fb
{
	namespace state
	{
		class StateInitialized : public State
		{
			public:
				StateInitialized(StateManager& context);

				virtual const char* getName() override;
				virtual void enter() override;
				virtual void exit() override;
		};
	}
}