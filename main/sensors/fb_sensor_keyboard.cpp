#include "fb_sensor_keyboard.hpp"


#include <cstring>
#include <ranges>
#include <stdlib.h>

#include "fb_globals.hpp"



#define _TASK_STACK_SIZE 3 * 1024
#define _TASK_PRIORITY 15



using namespace fb;
using namespace sensor;



template<int N>
const char* KeyboardSensor<N>::getName() const
{
	return "KeyboardSensor";
}

template<int N>
bool KeyboardSensor<N>::_doInit()
{
	h::Keyboard::instance()->setMinimumPressDuration(200);
	h::Keyboard::instance()->setPullPeriod(50);
	h::Keyboard::instance()->setRepeatPeriod(200);
	h::Keyboard::instance()->setConsumer([](const h::ButtonAction& val){
		h::ButtonAction* eventData = (h::ButtonAction*) std::malloc(sizeof(h::ButtonAction));
		assert(eventData);

		*eventData = val;

		event::Event e (
			event::EventGroup::KEYBOARD,
			std::to_underlying(KeyboardEventId::ACTION),
			static_cast<void*>(eventData),
			true);
		global::getEventManager()->pushEvent(e);
	});

	std::ranges::for_each(_pins, [](auto p){
		h::Keyboard::instance()->keyboardAddButton(static_cast<gpio_num_t>(p.first), p.second);
	});

	h::Keyboard::instance()->keyboardStart(_TASK_STACK_SIZE, _TASK_PRIORITY, 0);

	return true;
}

template<int N>
SensorIface::UpdateResult KeyboardSensor<N>::_doUpdate()
{
	return SensorIface::UpdateResult::OK;
}
