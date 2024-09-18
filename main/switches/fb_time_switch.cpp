#include "fb_time_switch.hpp"



#include "driver/gpio.h"



using namespace fb;
using namespace switches;



TimeSwitch::TimeSwitch(
	const clock::Time& startTime,
	const clock::Time& duration,
	int gpio
)
	: SwitchIface(&TimeSwitch::_condition, &TimeSwitch::_action),
	_startTime(startTime), _duration(duration), _gpio(gpio)
{
	gpio_config_t cfg = {
		.pin_bit_mask = 1ULL << gpio,
		.mode = GPIO_MODE_OUTPUT,
		.pull_up_en = GPIO_PULLUP_DISABLE,
		.pull_down_en = GPIO_PULLDOWN_DISABLE,
		.intr_type = GPIO_INTR_DISABLE,
	};
	assert(gpio_config(&cfg) == ESP_OK);
}

const char* TimeSwitch::getName()
{
	return "TimeSwitch";
}


bool TimeSwitch::_condition(SwitchIface* me)
{
	TimeSwitch* mePtr = reinterpret_cast<TimeSwitch*>(me);

	const clock::Timestamp current = clock::getCurrentTime();
	const clock::Timestamp end =
				static_cast<clock::Timestamp>(mePtr->_startTime) +
				static_cast<clock::Timestamp>(mePtr->_duration);
	
	return current >= mePtr->_startTime && current < end;
}

void TimeSwitch::_action(SwitchIface* me, bool value)
{
	TimeSwitch* mePtr = reinterpret_cast<TimeSwitch*>(me);

	gpio_set_level(static_cast<gpio_num_t>(mePtr->_gpio), value ? 1 : 0);
}
