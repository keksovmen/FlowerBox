#pragma once



#include "fb_state_manager.hpp"



namespace fb
{
	namespace state
	{
		/**
		 * @brief initializes all hardware that is required
		 */

		class StateProvision : public State
		{
			public:
				StateProvision(StateManager& context);

				virtual const char* getName() override;

				virtual void handleEvent(const event::Event& event) override;

				virtual void enter() override;
				virtual void exit() override;
		};
	}
}