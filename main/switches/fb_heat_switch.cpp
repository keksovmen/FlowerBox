#include "fb_heat_switch.hpp"

#include "driver/gpio.h"



using namespace fb;
using namespace switches;



HeatSwitch::HeatSwitch(sensor::TempreatureSensorTest* sensor, int sensorIndex,
	float onTemp, float offTemp, int gpio
)
	: SwitchIface(&HeatSwitch::_condition, &HeatSwitch::_action),
	_sensor(sensor), _sensorIndex(sensorIndex),
	_lowTemp(onTemp), _highTemp(offTemp), _gpio(gpio)
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

HeatSwitch::~HeatSwitch()
{

}

const char* HeatSwitch::getName()
{
	return "HeatSwitch";
}

void HeatSwitch::setSensorIndex(int index)
{
	_sensorIndex = index;
}

int HeatSwitch::getSensorIndex() const
{
	return _sensorIndex;
}

float HeatSwitch::getLowTemp() const
{
	return _lowTemp;
}

float HeatSwitch::getHighTemp() const
{
	return _highTemp;
}

int HeatSwitch::getGpio() const
{
	return _gpio;
}

const sensor::TempreatureSensorTest* HeatSwitch::getSensor() const
{
	return _sensor;
}

bool HeatSwitch::_checkTemperature()
{
	if(_getSensorValue() == sensor::TempreatureSensorTest::InvalidValue){
		//пока просто ждем когда сенсор инициализируется
		return false;
	}

	if(_isColling()){
		//охлождаемся, ждем когда температура упадет ниже минимума, тогда включаемся
		_setColling(_getSensorValue() > getLowTemp());
	}else{
		//нагреваемся, ждем когда температура вырастет чуть выше максимума
		_setColling(_getSensorValue() >= getHighTemp());
	}

	return !_isColling();
}

void HeatSwitch::_setColling(bool state)
{
	_colling = state;
}

bool HeatSwitch::_isColling() const
{
	return _colling;
}

float HeatSwitch::_getSensorValue() const
{
	return getSensor()->getValue(getSensorIndex());
}

bool HeatSwitch::_condition(SwitchIface* me)
{
	HeatSwitch* mePtr = reinterpret_cast<HeatSwitch*>(me);

	return mePtr->_checkTemperature();
}

void HeatSwitch::_action(SwitchIface* me, bool value)
{
	HeatSwitch* mePtr = reinterpret_cast<HeatSwitch*>(me);

	gpio_set_level(static_cast<gpio_num_t>(mePtr->_gpio), value ? 1 : 0);
}



FanSwitch::FanSwitch(sensor::TempreatureSensorTest* sensor, int sensorIndex,
						float lowTemp, float highTemp, int gpio)
	: HeatSwitch(sensor, sensorIndex, lowTemp, highTemp, gpio)
{

}

const char* FanSwitch::getName()
{
	return "FanSwitch";
}

bool FanSwitch::_checkTemperature()
{
	HeatSwitch::_checkTemperature();

	return _isColling();
}
