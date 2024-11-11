#include "fb_pins.hpp"

#include "driver/gpio.h"




using namespace fb;
using namespace pins;



const char* PinManager::getName() const
{
	return "PinManager";
}

void PinManager::handleEvent(const event::Event& event)
{
	FB_DEBUG_ENTER();
	if(event.groupId == event::EventGroup::BOOT){
		_onBootEvent(event);
	}
	FB_DEBUG_EXIT();
}

void PinManager::_onBootEvent(const event::Event& event)
{
	gpio_num_t _led_pins[] = {GPIO_NUM_5, GPIO_NUM_3, GPIO_NUM_4, GPIO_NUM_19, GPIO_NUM_18};

	for(int i = 0; i < sizeof(_led_pins) / sizeof(_led_pins[0]); i++){
		FB_DEBUG_LOG("Setting index = %d, gpio = %d", i, _led_pins[i]);

		assert(GPIO_IS_VALID_OUTPUT_GPIO(_led_pins[i]));

		gpio_config_t cfg = {
			.pin_bit_mask = 1ULL << _led_pins[i],
			.mode = GPIO_MODE_OUTPUT,
			.pull_up_en = GPIO_PULLUP_DISABLE,
			.pull_down_en = GPIO_PULLDOWN_DISABLE,
			.intr_type = GPIO_INTR_DISABLE,
		};
		gpio_config(&cfg);

		gpio_set_level(_led_pins[i], ((int)event.data) == i ? 1 : 0);
	}
}
