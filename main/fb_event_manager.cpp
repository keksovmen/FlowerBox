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
	const auto result = xQueueSend(_queue, &e, portMAX_DELAY);
	assert(result == pdPASS);
}

void EventManager::dispatchEvent()
{
	Event e;
	std::memset(&e, 0, sizeof(e));

	const auto result = xQueueReceive(_queue, &e, portMAX_DELAY);
	assert(result == pdPASS);

	fireEvent([&e](EventListener* l){l->handleEvent(e);});
}
