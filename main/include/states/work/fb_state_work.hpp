#pragma once



#include "fb_state_manager.hpp"



namespace fb
{
	namespace state
	{
		/**
		 * @brief Initializes stuff
		 */
		
		class StateWork : public CompositeState
		{
			public:
				StateWork(StateManager& context);

			protected:
				virtual void _onCompositeEnter();
				virtual void _onCompositeExit();
				virtual bool _onCompositeEventHandler(const event::Event& e);

			private:
				virtual std::unique_ptr<State> _createInitialState() override;
		};
	}
}