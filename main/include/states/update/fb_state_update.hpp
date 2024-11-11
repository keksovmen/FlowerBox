#pragma once



#include "fb_state_manager.hpp"



namespace fb
{
	namespace state
	{
		/**
		 * @brief Updates the firmware will restart the controller
		 */
		
		class StateUpdate : public State
		{
			public:
				StateUpdate(StateManager& context);

				virtual const char* getName() const override;

				virtual void handleEvent(const event::Event& event) override;

				virtual void enter() override;
				virtual void exit() override;
		};
	}
}