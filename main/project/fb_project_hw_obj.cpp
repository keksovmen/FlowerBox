#include "fb_project_hw_obj.hpp"

#include "fb_globals.hpp"
#include "fb_keyboard_handler.hpp"
#include "fb_pins.hpp"
#include "fb_project_settings.hpp"



using namespace fb;
using namespace project;



//сенсоры туть
static sensor::TemperatureSensorArray<1> _sensorArrayDS18(pins::PIN_SENSOR_DS18);
static sensor::TemperatureSensor* _sensorDS18Outside = _sensorArrayDS18.getSensor(0);
static sensor::SensorAht20 _sensorAhtInside(I2C_NUM_0, pins::PIN_IIC_SDA, pins::PIN_IIC_SCL);

//переключатели туть
static switches::TimeSwitch _switchLight(clock::Time(0, 0, 0), clock::Time(0, 0, 0), pins::PIN_LIGHT);
//change to reference instead of a pointer senor argument
static switches::HeatSwitch _switchHeating(_sensorDS18Outside, 28.5, 29, pins::PIN_GREEN_LED);
static switches::FanSwitch _switchFan(_sensorDS18Outside, 30, 31, pins::PIN_FAN);

//сервисы туть
static sensor::SensorService _sensorService;
static switches::SwitchService _swithService;

//склады туть
static sensor::SensorStorage _sensorStorage;

//прочее туть
static keyboard::KeyboardHandler _keyboardHandler;



static void _init_from_settings()
{
	_switchLight.setStartTime(settings::getLightStartTime());
	_switchLight.setEndTime(settings::getLightEndTime());
}



void project::initHwObjs()
{
	_sensorService.addSensor(&getHwTempSensors());
	_sensorService.addSensor(&getHwDS18Sensor());
	_sensorService.addSensor(&getHwAhtSensor());

	_swithService.addSwitch(&getHwLightSwitch());
	_swithService.addSwitch(&getHwHeatSwitch());
	_swithService.addSwitch(&getHwFanSwitch());

	//init from settings
	_init_from_settings();

	//register key handler for dropping WIFI settings
	global::getEventManager()->attachListener(&_keyboardHandler);
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
