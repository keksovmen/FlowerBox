#pragma once



#include <memory>
#include <vector>

#include "fb_debug.hpp"
#include "fb_subject.hpp"

#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"



namespace fb
{
	namespace event
	{
		enum class EventGroup : int
		{
			BOOT,
			TRANSITION,
			WIFI,
			STATE,
			TIMERS,
			CLOCK,
			UPDATE,
			SENSOR,
			PROVISION,
		};



		struct Event
		{
			EventGroup groupId;
			int eventId;		//см различные .hpp файлы зависящие от группы
			void* data;
		};



		class EventListener
		{
			public:
				virtual void handleEvent(const Event& event) = 0;
		};



		class EventManager : public debug::Named, public subject::BaseSubject<EventListener, 3>
		{
			public:
				explicit EventManager(int queueSize = 10);
				virtual ~EventManager();

				virtual const char* getName() const override;

				void pushEvent(const Event& e);
				void dispatchEvent();
			
			private:
				QueueHandle_t _queue;
		};



		const char* eventGroupToStr(const EventGroup& group);
	}
}