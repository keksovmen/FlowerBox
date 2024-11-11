#pragma once



#include "fb_state_manager.hpp"



namespace fb
{
	namespace state
	{
		/**
		 * @brief WIFI okay
		 */
		
		class StateWifiConnected : public State
		{
			public:
				StateWifiConnected(StateManager& context);

				virtual const char* getName() const override;

				virtual void handleEvent(const event::Event& event) override;

				virtual void enter() override;
				virtual void exit() override;
		};
	}
}