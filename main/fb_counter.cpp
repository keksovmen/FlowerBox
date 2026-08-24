#include "fb_counter.hpp"



using namespace fb;
using namespace util;



Counter::Counter(int target, int value, OnAchieveTarget targetCb)
	: _resultCb(targetCb), _target(target), _value(value)
{

}

const char* Counter::getName() const
{
	return "Counter";
}

void Counter::changeMultiplier()
{
	_multiplier *= 10;
	_multiplier = (_multiplier > 100) ? 1 : _multiplier;
	FB_DEBUG_LOG_I_OBJ("Multiplier = %d", _multiplier);
}

void Counter::increment()
{
	int val = _value + _multiplier;
	val = (val >= 1000) ? val - 1000 : val;

	setValue(val);
}

void Counter::setTarget(int target)
{
	if(target < 0 || target > 999){
		FB_DEBUG_LOG_W_OBJ("Illegal target = %d", target);
		return;
	}

	_target = target;
	FB_DEBUG_LOG_I_OBJ("New target = %d", target);

	_checkCondition();
}

void Counter::setValue(int value)
{
	if(value < 0 || value > 999){
		FB_DEBUG_LOG_W_OBJ("Illegal value = %d", value);
		return;
	}

	_value = value;
	FB_DEBUG_LOG_I_OBJ("New value = %d", _value);

	_checkCondition();
}

int Counter::getValue() const
{
	return _value;
}

int Counter::getMultiplier() const
{
	return _multiplier;
}

void Counter::_checkCondition()
{
	if(_target == _value){
		FB_DEBUG_LOG_I_OBJ("Target reached: %d == %d", _target, _value);
		std::invoke(_resultCb, _value);
	}
}
