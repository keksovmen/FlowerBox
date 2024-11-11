#pragma once



#include "fb_state_manager.hpp"



namespace fb
{
	namespace state
	{
		/**
		 * @brief Initialize HW sensors
		 */
		
		class StateSensorInit : public State
		{
			public:
				StateSensorInit(StateManager& context);

				virtual void enter() override;
				virtual void exit() override;

				virtual void handleEvent(const event::Event& event) override;

				virtual const char* getName() const override;
		};
	}
}