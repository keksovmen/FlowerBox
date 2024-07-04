#pragma once



#include "fb_state_manager.hpp"



namespace fb
{
	namespace state
	{
		/**
		 * @brief Tries to fix WIFI state
		 */
		
		class StateWifiLost : public State
		{
			public:
				StateWifiLost(StateManager& context);

				virtual const char* getName() override;

				virtual void handleEvent(const event::Event& event) override;

				virtual void enter() override;
				virtual void exit() override;
		};
	}
}