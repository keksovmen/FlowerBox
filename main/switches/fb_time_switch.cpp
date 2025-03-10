#include "fb_time_switch.hpp"



#include "driver/gpio.h"



using namespace fb;
using namespace switches;



TimeSwitch::TimeSwitch(
	const clock::Time& startTime,
	const clock::Time& endTime,
	wrappers::WrapperIface* wrapper
)
	: SwitchIface(&TimeSwitch::_condition, &TimeSwitch::_action),
	_startTime(startTime), _endTime(endTime), _wrapper(wrapper)
{
	wrapper->init();
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
	return _startTime > _endTime;
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
	mePtr->_wrapper->setValue(value);
}
