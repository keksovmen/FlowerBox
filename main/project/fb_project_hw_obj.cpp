#include "fb_project_hw_obj.hpp"

#include "fb_pins.hpp"



using namespace fb;
using namespace project;



//сенсоры туть
static sensor::TemperatureSensorArray<1> _sensorArrayDS18(pins::PIN_SENSOR_TEMPERATURE);
static sensor::TemperatureSensor* _sensorDS18Outside = _sensorArrayDS18.getSensor(0);
static sensor::SensorAht20 _sensorAhtInside(I2C_NUM_0, pins::PIN_IIC_SDA, pins::PIN_IIC_SCL);

//переключатели туть
static switches::TimeSwitch _switchLight(clock::Time(0, 0, 0), clock::Time(0, 1, 0), pins::PIN_RED_LED);
//change to reference instead of a pointer senor argument
static switches::HeatSwitch _switchHeating(_sensorDS18Outside, 28.5, 29, pins::PIN_GREEN_LED);
static switches::FanSwitch _switchFan(_sensorDS18Outside, 30, 31, pins::PIN_COOL_LED);

//сервисы туть
static sensor::SensorService _sensorService;
static switches::SwitchService _swithService;

//склады туть
static sensor::SensorStorage _sensorStorage;



void project::initHwObjs()
{
	_sensorService.addSensor(&getHwTempSensors());
	_sensorService.addSensor(&getHwDS18Sensor());
	_sensorService.addSensor(&getHwAhtSensor());

	_swithService.addSwitch(&getHwLightSwitch());
	_swithService.addSwitch(&getHwHeatSwitch());
	_swithService.addSwitch(&getHwFanSwitch());
}

sensor::TemperatureSensorArrayI& project::getHwTempSensors()
{
	return _sensorArrayDS18;
}

sensor::TemperatureSensor& project::getHwDS18Sensor()
{
	return *_sensorDS18Outside;
}

sensor::SensorAht20& project::getHwAhtSensor()
{
	return _sensorAhtInside;
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
