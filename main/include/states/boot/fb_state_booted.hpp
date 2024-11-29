#pragma once



#include "fb_state_manager.hpp"



namespace fb
{
	namespace state
	{
		/**
		 * @brief Initialize HW, WIFI, Time
		 */
		
		class StateBooted : public CompositeState
		{
			public:
				StateBooted(StateManager& context);

			protected:
				virtual void _onCompositeEnter() override;

				virtual bool _onCompositeEventHandler(const event::Event& event) override;
			
			private:
				virtual std::unique_ptr<State> _createInitialState() override;
		};
	}
}