#pragma once


#include <memory>
#include <string>

#include "fb_debug.hpp"
#include "fb_event_manager.hpp"



namespace fb
{
	namespace state
	{
		class StateManager;



		class State : public debug::Named, public event::EventListener
		{
			public:
				State(StateManager& context);
				~State();

				virtual void enter() = 0;
				virtual void exit() = 0;

				StateManager& getContext();
			
			private:
				StateManager& _context;
		};



		class StateManager : public debug::Named, public event::EventListener
		{
			public:
				StateManager(const std::string& name);

				virtual const char* getName() override;

				virtual void handleEvent(const event::Event& event) override;
				
				/**
				 * @brief вызовет на заданном состоянии enter
				 */

				void init(std::unique_ptr<State>&& initialState);

				/**
				 * @brief вызовет на старом exit() на новом enter
				 * 
				 * @param nextState 
				 */

				void transition(std::unique_ptr<State>&& nextState);

				/**
				 * @brief вызовет на старом exit
				 */

				void deinit();
			
			private:
				const std::string _name;
				std::unique_ptr<State> _currentState;
		};
	}
}