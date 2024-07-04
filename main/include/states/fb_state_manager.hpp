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



		class State : public virtual debug::Named, public virtual event::EventListener
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



		class StateManager : public virtual debug::Named, public virtual event::EventListener
		{
			public:
				StateManager(const std::string& name);
				~StateManager();

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
				 * @brief вызовет на старом exit и удалит его
				 */

				void deinit();
			
			private:
				const std::string _name;
				std::unique_ptr<State> _currentState;
		};



		class CompositeState : public State, public StateManager
		{
			public:
				CompositeState(StateManager& context, const std::string& name);
				// ~CompositeState();

				virtual void enter() override;
				virtual void exit() override;

				virtual void handleEvent(const event::Event& event) override;
			
			protected:
				virtual void _onCompositeEnter();
				virtual void _onCompositeExit();
				//return true if MUST send event to child false to end event
				virtual bool _onCompositeEventHandler(const event::Event& event);
			
			private:
				virtual std::unique_ptr<State> _createInitialState() = 0;
		};
	}
}