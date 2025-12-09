#include "fb_sensor_keyboard.hpp"


#include <cstring>
#include <stdlib.h>

#include "fb_globals.hpp"

#include "esp_sleep.h"



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
			static_cast<int>(KeyboardEventId::ACTION),
			static_cast<void*>(eventData),
			true);
		global::getEventManager()->pushEvent(e);
	});

	std::for_each(_pins.begin(), _pins.end(), [](auto p){
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



static void IRAM_ATTR _gpio_isr(void* arg) {
	static TickType_t previousTick = 0;
	//debounce
	if(pdTICKS_TO_MS(xTaskGetTickCountFromISR() - previousTick) < 50){
		return;
	}
	previousTick = xTaskGetTickCountFromISR();
	// const int gpio_int = reinterpret_cast<int>(arg);
	// const gpio_num_t gpio = static_cast<gpio_num_t>(gpio_int);
	// const bool level = gpio_get_level(gpio);

	global::getTimeScheduler()->addActionDelayedIrq(
		[=](){
			h::Keyboard::instance()->tick();
		}, 50, 250, false
	);
}

template<int N>
const char* KeyboardSensorIrq<N>::getName() const
{
	return "KeyboardSensorIrq";
}

template<int N>
bool KeyboardSensorIrq<N>::_doInit()
{
	h::Keyboard::instance()->setMinimumPressDuration(200);
	h::Keyboard::instance()->setRepeatPeriod(200);
	h::Keyboard::instance()->setConsumer([](const h::ButtonAction& val){
		h::ButtonAction* eventData = (h::ButtonAction*) std::malloc(sizeof(h::ButtonAction));
		assert(eventData);

		*eventData = val;

		event::Event e (
			event::EventGroup::KEYBOARD,
			static_cast<int>(KeyboardEventId::ACTION),
			static_cast<void*>(eventData),
			true);
		global::getEventManager()->pushEvent(e);
	});

	gpio_install_isr_service(ESP_INTR_FLAG_IRAM);
	esp_sleep_enable_gpio_wakeup();

	std::for_each(_pins.begin(), _pins.end(), [](auto p){
		auto pin = static_cast<gpio_num_t>(p.first);

		h::Keyboard::instance()->keyboardAddButton(pin, p.second);

		gpio_set_intr_type(pin, GPIO_INTR_ANYEDGE);
		gpio_intr_enable(pin);
		gpio_isr_handler_add(pin, &_gpio_isr, reinterpret_cast<void*>(pin));

		//TODO: for some reason system is crashed in enormous memory alloc 
		// gpio_wakeup_enable(pin, GPIO_INTR_LOW_LEVEL);
		
		gpio_sleep_set_direction(pin, GPIO_MODE_INPUT);
		gpio_sleep_set_pull_mode(pin, GPIO_PULLUP_ONLY);
		gpio_sleep_sel_en(pin);
	});

	h::Keyboard::instance()->tick();

	return true;
}

template<int N>
SensorIface::UpdateResult KeyboardSensorIrq<N>::_doUpdate()
{
	h::Keyboard::instance()->tick();

	return SensorIface::UpdateResult::OK;
}
