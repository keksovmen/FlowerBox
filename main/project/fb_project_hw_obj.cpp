#include "fb_project_hw_obj.hpp"

#include "fb_pins.hpp"



using namespace fb;
using namespace project;



//сенсоры туть
static sensor::TemperatureSensorArray<2> _sensorArrayTemp(pins::PIN_SENSOR_TEMPERATURE);
static sensor::TemperatureSensor* _sensorTemperatureInside = _sensorArrayTemp.getSensor(0);
static sensor::TemperatureSensor* _sensorTemperatureOutside = _sensorArrayTemp.getSensor(1);

//переключатели туть
static switches::TimeSwitch _switchLight(clock::Time(0, 0, 0), clock::Time(0, 1, 0), pins::PIN_BLUE_LED);
//change to reference instead of a pointer senor argument
static switches::HeatSwitch _switchHeating(_sensorTemperatureInside, 28.5, 29, pins::PIN_GREEN_LED);
static switches::FanSwitch _switchFan(_sensorTemperatureOutside, 30, 31, pins::PIN_COOL_LED);

//сервисы туть
static sensor::SensorService _sensorService;
static switches::SwitchService _swithService;

//склады туть
static sensor::SensorStorage _sensorStorage;



void project::initHwObjs()
{
	_sensorService.addSensor(&getHwTempSensors());
	_sensorService.addSensor(&getHwInsideTempSensor());
	_sensorService.addSensor(&getHwOutsideTempSensor());

	_swithService.addSwitch(&getHwLightSwitch());
	_swithService.addSwitch(&getHwHeatSwitch());
	_swithService.addSwitch(&getHwFanSwitch());
}

sensor::TemperatureSensorArrayI& project::getHwTempSensors()
{
	return _sensorArrayTemp;
}

sensor::TemperatureSensor& project::getHwInsideTempSensor()
{
	return *_sensorTemperatureInside;
}

sensor::TemperatureSensor& project::getHwOutsideTempSensor()
{
	return *_sensorTemperatureOutside;
}

switches::TimeSwitch& project::getHwLightSwitch()
{
	return _switchLight;
}

switches::HeatSwitch& project::getHwHeatSwitch()
{
	return _switchHeating;
}

switches::FanSwitch& project::getHwFanSwitch()
{
	return _switchFan;
}

sensor::SensorService& project::getHwSensorService()
{
	return _sensorService;
}

switches::SwitchService& project::getHwSwitchService()
{
	return _swithService;
}

sensor::SensorStorage& project::getHwSensorStorage()
{
	return _sensorStorage;
}
