#include "fb_time_switch.hpp"



#include "driver/gpio.h"



using namespace fb;
using namespace switches;



TimeSwitch::TimeSwitch(
	const clock::Time& startTime,
	const clock::Time& endTime,
	int gpio
)
	: SwitchIface(&TimeSwitch::_condition, &TimeSwitch::_action),
	_startTime(startTime), _endTime(endTime), _gpio(gpio)
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

const char* TimeSwitch::getName() const
{
	return "TimeSwitch";
}

void TimeSwitch::setStartTime(clock::Timestamp seconds)
{
	_startTime = clock::Time(seconds);
}

void TimeSwitch::setEndTime(clock::Timestamp seconds)
{
	_endTime = clock::Time(seconds);
}

const clock::Time& TimeSwitch::getStartTime() const
{
	return _startTime;
}

const clock::Time& TimeSwitch::getEndTime() const
{
	return _endTime;
}

bool TimeSwitch::_isOverflow() const
{
	return _startTime < _endTime;
}

bool TimeSwitch::_condition(SwitchIface* me)
{
	TimeSwitch* mePtr = reinterpret_cast<TimeSwitch*>(me);

	const clock::Timestamp current = clock::getCurrentTime();

	//if start and end are equals we consider as not working
	if(mePtr->_startTime == mePtr->_endTime){
		return false;
	}

	if(mePtr->_isOverflow()){
		return current >= mePtr->_startTime || current < mePtr->_endTime;
	}else{
		return current >= mePtr->_startTime && current < mePtr->_endTime;
	}
}

void TimeSwitch::_action(SwitchIface* me, bool value)
{
	TimeSwitch* mePtr = reinterpret_cast<TimeSwitch*>(me);

	gpio_set_level(static_cast<gpio_num_t>(mePtr->_gpio), value ? 1 : 0);
}
