#include "fb_event_manager.hpp"

#include <cstring>
#include <stdlib.h>



using namespace fb;
using namespace event;



EventManager::EventManager(int queueSize)
{
	_queue = xQueueCreate(queueSize, sizeof(Event));
	assert(_queue);
}

EventManager::~EventManager()
{
	vQueueDelete(_queue);
}

const char* EventManager::getName() const
{
	return "EventManager";
}

void EventManager::pushEvent(const Event& e)
{
	FB_DEBUG_LOG_I_OBJ("Push event: %d = %s, event cmd %d, arg %d, free %d", static_cast<int>(e.groupId), eventGroupToStr(e.groupId), e.eventId, reinterpret_cast<int>(e.data), e.freeMemory);
	
	const auto result = xQueueSend(_queue, &e, portMAX_DELAY);
	assert(result == pdPASS);
}

void EventManager::dispatchEvent()
{
	FB_DEBUG_ENTER_W_OBJ();

	Event e{};

	const auto result = xQueueReceive(_queue, &e, portMAX_DELAY);
	assert(result == pdPASS);
	FB_DEBUG_LOG_I_OBJ("Event group: %d = %s, event cmd %d, arg %d, free %d", static_cast<int>(e.groupId), eventGroupToStr(e.groupId), e.eventId, reinterpret_cast<int>(e.data), e.freeMemory);

	fireEvent([&e](EventListener* l){l->handleEvent(e);});
	if(e.freeMemory){
		std::free(e.data);
	}
}



const char* event::eventGroupToStr(const EventGroup& group)
{
	const char* names[] = {
		"BOOT",
		"TRANSITION",
		"WIFI",
		"STATE",
		"TIMERS",
		"CLOCK",
		"UPDATE",
		"SENSOR",
		"PROVISION",
		"KEYBOARD",
	};
	assert(static_cast<int>(group) < sizeof(names) / sizeof(names[0]));

	return names[static_cast<int>(group)];
}