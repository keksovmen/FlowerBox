#pragma once



#include "fb_state_manager.hpp"



namespace fb
{
	namespace state
	{
		/**
		 * @brief Initialize RTC time from network
		 */

		class StateTimeInit : public State
		{
			public:
				StateTimeInit(StateManager& context);

				virtual const char* getName() const override;

				virtual void handleEvent(const event::Event& event) override;

				virtual void enter() override;
				virtual void exit() override;
		};
	}
}