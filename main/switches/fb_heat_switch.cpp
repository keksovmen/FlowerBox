#include "fb_heat_switch.hpp"

#include "driver/gpio.h"



using namespace fb;
using namespace switches;



RangeSwitch::RangeSwitch(float lowValue, float highValue, ReadCb read, ActionCb action, bool inverseFlag)
	: SwitchIface(&RangeSwitch::_condition, &RangeSwitch::_action),
	_readCb(read), _actionCb(action), _inverseFlag(inverseFlag),
	_lowValue(lowValue), _highValue(highValue)
{

}

const char* RangeSwitch::getName() const
{
	return "RangeSwitch";
}

void RangeSwitch::setLowValue(float value)
{
	_lowValue = value;
}

void RangeSwitch::setHighValue(float value)
{
	_highValue = value;
}

float RangeSwitch::getLowValue() const
{
	return _lowValue;
}

float RangeSwitch::getHighValue() const
{
	return _highValue;
}

bool RangeSwitch::checkValues()
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

	const bool result = !_isColling();

	return _inverseFlag ? !result : result;
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

	return mePtr->checkValues();
}

void RangeSwitch::_action(SwitchIface* me, bool value)
{
	auto* mePtr = reinterpret_cast<RangeSwitch*>(me);

	std::invoke(mePtr->_actionCb, value);
}



SensorSwitch::SensorSwitch(sensor::SensorIface* sens,
	float onTemp, float offTemp, wrappers::WrapperIface* wrapper, bool inverseFlag
)
	: RangeSwitch(onTemp, offTemp,
		[this](){return _sensor->getValue() == sensor::TemperatureSensor::InvalidValue ?
			RangeSwitch::INVALID_VALUE : _sensor->getValue();},
		[this](bool val){_wrapper->setValue(val ? _speed : 0);}, inverseFlag),
	_sensor(sens),
	_wrapper(wrapper)
{
	_wrapper->init();
}

const char* SensorSwitch::getName() const
{
	return "SensorSwitch";
}

void SensorSwitch::setSpeed(int speed)
{
	_speed = speed;
	if(isOn()){
		_wrapper->setValue(speed);
	}
}

int SensorSwitch::getSpeed() const
{
	return _speed;
}



FanSwitch::FanSwitch(sensor::SensorAht20* sensor,
	float lowTemp, float highTemp,
	float lowHumidity, float highHumidity,
	wrappers::WrapperIface* wrapper)
	: SwitchIface(&FanSwitch::_condition, &FanSwitch::_action),
	_sensor(sensor), _wrapper(wrapper),
	_tempSwitch(lowTemp, highTemp,
		[this](){return _sensor->getTemperature() == sensor::SensorAht20::InvalidValue ?
			RangeSwitch::INVALID_VALUE : _sensor->getTemperature();},
		[this](bool val){_wrapper->setValue(val ? _speed : 0);}, true),
	_humSwitch(lowHumidity, highHumidity,
		[this](){return _sensor->getHumidity() == sensor::SensorAht20::InvalidValue ?
			RangeSwitch::INVALID_VALUE : _sensor->getHumidity();},
		[this](bool val){_wrapper->setValue(val ? _speed : 0);}, true)
{
	wrapper->init();
}
	
const char* FanSwitch::getName() const
{
	return "FanSwitch";
}

void FanSwitch::setSpeed(int speed)
{
	_speed = speed;

	if(isOn()){
		_wrapper->setValue(speed);
	}
}

int FanSwitch::getSpeed() const
{
	return _speed;
}

void FanSwitch::setTempLowValue(float value)
{
	_tempSwitch.setLowValue(value);
}

void FanSwitch::setTempHighValue(float value)
{
	_tempSwitch.setHighValue(value);
}

void FanSwitch::setHumLowValue(float value)
{
	_humSwitch.setLowValue(value);
}

void FanSwitch::setHumHighValue(float value)
{
	_humSwitch.setHighValue(value);
}

float FanSwitch::getTempLowValue() const
{
	return _tempSwitch.getLowValue();
}

float FanSwitch::getTempHighValue() const
{
	return _tempSwitch.getHighValue();
}

float FanSwitch::getHumLowValue() const
{
	return _humSwitch.getLowValue();
}

float FanSwitch::getHumHighValue() const
{
	return _humSwitch.getHighValue();
}

bool FanSwitch::_condition(SwitchIface* me)
{
	auto* self = static_cast<FanSwitch*>(me);

	return self->_humSwitch.checkValues() || self->_tempSwitch.checkValues();
}

void FanSwitch::_action(SwitchIface* me, bool value)
{
	auto* self = static_cast<FanSwitch*>(me);

	self->_wrapper->setValue(value ? self->getSpeed() : 0);
}
