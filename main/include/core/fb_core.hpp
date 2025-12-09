#pragma once



#include "fb_core_box_obj.hpp"
#include "fb_core_hw_obj.hpp"
#include "fb_core_maper_obj.hpp"
#include "fb_debug.hpp"
#include "fb_event_manager.hpp"



namespace fb
{
	namespace project
	{
		class Core : public debug::Named, public event::EventListener
		{
			public:
				virtual ~Core() = default;
		};



		class BaseCore : public Core
		{
			public:
				using EventHandler = std::function<void(const event::Event&)>;



				BaseCore(const EventHandler& handler, std::string name)
					: _eventHandler(handler), _name(name)
				{

				}

				virtual const char* getName() const override
				{
					return _name.c_str();
				}

				virtual void handleEvent(const event::Event& event) override
				{
					std::invoke(_eventHandler, event);
				}

			private:
				EventHandler _eventHandler;
				std::string _name; 
		};



		struct CoreInfo
		{
			Core& core;
			bool requiresTime;
			bool requireSensorService = true;
			bool requireSwitchService = true;
		};



		void bootInit();
		const CoreInfo& getInfo(); 
	}
}