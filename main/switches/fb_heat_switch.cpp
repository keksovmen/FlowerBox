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
		_setColling(_getSensorValue() > _getTargetLowValue());
	}else{
		//нагреваемся, ждем когда температура вырастет чуть выше максимума
		_setColling(_getSensorValue() >= _getTargetHighValue());
	}
	// FB_DEBUG_LOG_E_OBJ("Coolin=%d, inverse=%d, result=%d",
	// 	_isColling(), _inverseFlag, _inverseFlag ? _isColling() : !_isColling());

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

float RangeSwitch::_getTargetLowValue() const
{
	return _lowValue;
}

float RangeSwitch::_getTargetHighValue() const
{
	return _highValue;
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



DayNightRangeSwitch::DayNightRangeSwitch(
			float lowValue, float highValue,
			ReadCb read, ActionCb action,
			int delta, bool inverseFlag)
	: RangeSwitch(lowValue, highValue, read, action, inverseFlag),
	_delta(delta)
{

}

const char* DayNightRangeSwitch::getName() const
{
	return "DayNightRangeSwitch";
}

void DayNightRangeSwitch::setDelta(float delta)
{
	_delta = delta;
}

float DayNightRangeSwitch::getDelta() const
{
	return _delta;
}

void DayNightRangeSwitch::setDayStartTime(clock::Timestamp seconds)
{
	_startTime = clock::Time(seconds);
}

void DayNightRangeSwitch::setDayEndTime(clock::Timestamp seconds)
{
	_endTime = clock::Time(seconds);
}

const clock::Time& DayNightRangeSwitch::getDayStartTime() const
{
	return _startTime;
}

const clock::Time& DayNightRangeSwitch::getDayEndTime() const
{
	return _endTime;
}

float DayNightRangeSwitch::_getTargetLowValue() const
{
	// FB_DEBUG_LOG_E_OBJ("D=%.2f, Low=%.2f, High=%.2f, Current=%.2f, R=%.2f",
	// 	getDelta(), getLowValue(), getHighValue(), _getSensorValue(),
	// 	getLowValue() - (_isDay() ? 0 : getDelta()));
	return getLowValue() - (_isDay() ? 0 : getDelta());
}

float DayNightRangeSwitch::_getTargetHighValue() const
{
	// FB_DEBUG_LOG_E_OBJ("D=%.2f, Low=%.2f, High=%.2f, Current=%.2f, R=%.2f",
	// 	getDelta(), getLowValue(), getHighValue(), _getSensorValue(),
	// 	getHighValue() - (_isDay() ? 0 : getDelta()));
	return getHighValue() - (_isDay() ? 0 : getDelta());
}

bool DayNightRangeSwitch::_isDay() const
{
	if(_startTime == _endTime){
		return true;
	}

	const clock::Timestamp current = clock::getCurrentTime();

	if(_startTime > _endTime){
		return current >= _startTime || current < _endTime;
	}else{
		return current >= _startTime && current < _endTime;
	}
}



SensorSwitch::SensorSwitch(sensor::SensorIface* sens,
	float onTemp, float offTemp, wrappers::WrapperIface* wrapper, bool inverseFlag
)
	: DayNightRangeSwitch(onTemp, offTemp,
		[this](){return _sensor->getValue() == sensor::SensorIface::InvalidValue ?
			RangeSwitch::INVALID_VALUE : _sensor->getValue();},
		[this](bool val){_wrapper->setValue(val ? _speed : 0);}, 0, inverseFlag),
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
		[this](bool val){_wrapper->setValue(val ? _speed : 0);}, 0, true)
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

void FanSwitch::setDayStartTime(clock::Timestamp seconds)
{
	_humSwitch.setDayStartTime(seconds);
}

void FanSwitch::setDayEndTime(clock::Timestamp seconds)
{
	_humSwitch.setDayEndTime(seconds);
}

const clock::Time& FanSwitch::getDayStartTime() const
{
	return _humSwitch.getDayStartTime();
}

const clock::Time& FanSwitch::getDayEndTime() const
{
	return _humSwitch.getDayEndTime();
}

void FanSwitch::setDelta(float delta)
{
	return _humSwitch.setDelta(delta);
}

float FanSwitch::getDelta() const
{
	return _humSwitch.getDelta();
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
