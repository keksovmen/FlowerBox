#pragma once



#include "fb_state_manager.hpp"



namespace fb
{
	namespace state
	{
		/**
		 * @brief initializes all hardware that is required
		 */

		class StateHwInit : public State
		{
			public:
				StateHwInit(StateManager& context);

				virtual const char* getName() const override;

				virtual void handleEvent(const event::Event& event) override;

				virtual void enter() override;
				virtual void exit() override;
		};
	}
}