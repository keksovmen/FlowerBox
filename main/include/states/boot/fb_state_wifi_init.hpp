#pragma once



#include "fb_state_manager.hpp"



namespace fb
{
	namespace state
	{
		/**
		 * @brief Initialize proper wifi mode from settings or default
		 */
		
		class StateWifiInit : public State
		{
			public:
				StateWifiInit(StateManager& context);

				virtual const char* getName() override;

				virtual void handleEvent(const event::Event& event) override;

				virtual void enter() override;
				virtual void exit() override;
		};
	}
}