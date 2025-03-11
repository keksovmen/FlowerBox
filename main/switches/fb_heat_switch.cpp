#include "fb_heat_switch.hpp"

#include "driver/gpio.h"



using namespace fb;
using namespace switches;



RangeSwitch::RangeSwitch(float lowValue, float highValue, ReadCb read, ActionCb action)
	: SwitchIface(&RangeSwitch::_condition, &RangeSwitch::_action),
	_readCb(read), _actionCb(action), _lowValue(lowValue), _highValue(highValue)
{

}

const char* RangeSwitch::getName() const
{
	return "RangeSwitch";
}

float RangeSwitch::getLowValue() const
{
	return _lowValue;
}

float RangeSwitch::getHighValue() const
{
	return _highValue;
}

bool RangeSwitch::_checkValues()
{
	if(_getSensorValue() == INVALID_VALUE){
		//пока просто ждем когда сенсор инициализируется
		return false;
	}

	if(_isColling()){
		//охлождаемся, ждем когда температура упадет ниже минимума, тогда включаемся
		_setColling(_getSensorValue() > getLowValue());
	}else{
		//нагреваемся, ждем когда температура вырастет чуть выше максимума
		_setColling(_getSensorValue() >= getHighValue());
	}

	return !_isColling();
}

void RangeSwitch::_setColling(bool state)
{
	_colling = state;
}

bool RangeSwitch::_isColling() const
{
	return _colling;
}

float RangeSwitch::_getSensorValue() const
{
	return std::invoke(_readCb);
}

bool RangeSwitch::_condition(SwitchIface* me)
{
	auto* mePtr = reinterpret_cast<RangeSwitch*>(me);

	return mePtr->_checkValues();
}

void RangeSwitch::_action(SwitchIface* me, bool value)
{
	auto* mePtr = reinterpret_cast<RangeSwitch*>(me);

	std::invoke(mePtr->_actionCb, value);
}



HeatSwitch::HeatSwitch(sensor::SensorIface* sens,
	float onTemp, float offTemp, wrappers::WrapperIface* wrapper
)
	: RangeSwitch(onTemp, offTemp,
		[this](){return _sensor->getValue() == sensor::TemperatureSensor::InvalidValue ?
			RangeSwitch::INVALID_VALUE : _sensor->getValue();},
		[this](bool val){_wrapper->setValue(val ? _speed : 0);}),
	_sensor(sens),
	_wrapper(wrapper)
{
	_wrapper->init();
}

const char* HeatSwitch::getName() const
{
	return "HeatSwitch";
}

void HeatSwitch::setSpeed(int speed)
{
	_speed = speed;
	if(isOn()){
		_wrapper->setValue(speed);
	}
}

int HeatSwitch::getSpeed() const
{
	return _speed;
}




FanSwitch::FanSwitch(sensor::SensorIface* sensor,
					float lowTemp, float highTemp, wrappers::WrapperIface* wrapper)
	: HeatSwitch(sensor, lowTemp, highTemp, wrapper)
{

}

const char* FanSwitch::getName() const
{
	return "FanSwitch";
}

bool FanSwitch::_checkValues()
{
	return !HeatSwitch::_checkValues();
}