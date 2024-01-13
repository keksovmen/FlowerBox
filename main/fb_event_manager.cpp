#include "fb_event_manager.hpp"

#include <cstring>



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

const char* EventManager::getName()
{
	return "EventManager";
}

void EventManager::pushEvent(const Event& e)
{
	FB_DEBUG_LOG("Push event: %d = %s, event cmd %d, arg %d", static_cast<int>(e.groupId), eventGroupToStr(e.groupId), e.eventId, reinterpret_cast<int>(e.data));
	
	const auto result = xQueueSend(_queue, &e, portMAX_DELAY);
	assert(result == pdPASS);
}

void EventManager::dispatchEvent()
{
	FB_DEBUG_ENTER();

	Event e;
	std::memset(&e, 0, sizeof(e));

	const auto result = xQueueReceive(_queue, &e, portMAX_DELAY);
	assert(result == pdPASS);
	FB_DEBUG_LOG("Event group: %d = %s, event cmd %d, arg %d", static_cast<int>(e.groupId), eventGroupToStr(e.groupId), e.eventId, reinterpret_cast<int>(e.data));

	fireEvent([&e](EventListener* l){l->handleEvent(e);});

	FB_DEBUG_EXIT();
}



const char* event::eventGroupToStr(const EventGroup& group)
{
	const char* names[] = {
		"BOOT",
		"WIFI",
		"STATE",
		"TIMERS",
		"CLOCK",
		"UPDATE",
	};
	assert(static_cast<int>(group) < sizeof(names) / sizeof(names[0]));

	return names[static_cast<int>(group)];
}