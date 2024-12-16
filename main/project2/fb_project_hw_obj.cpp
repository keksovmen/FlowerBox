#include "fb_project_hw_obj.hpp"

#include "fb_pins.hpp"



using namespace fb;
using namespace project;



//сенсоры туть
static sensor::Mp3Sensor _mp3Sensor(16, 17);

// //переключатели туть
// static switches::TimeSwitch _switchLight(clock::Time(0, 0, 0), clock::Time(0, 1, 0), pins::PIN_BLUE_LED);
// //change to reference instead of a pointer senor argument
// static switches::HeatSwitch _switchHeating(_sensorTemperatureInside, 28.5, 29, pins::PIN_GREEN_LED);
static switches::RgbSwitch _rgbSwitch(LEDC_TIMER_0, LEDC_CHANNEL_0,
			pins::PIN_RED_LED, pins::PIN_GREEN_LED, pins::PIN_BLUE_LED);
			

// //сервисы туть
static sensor::SensorService _sensorService;
static switches::SwitchService _swithService;

//склады туть
static sensor::SensorStorage _sensorStorage;



void project::initHwObjs()
{
	// _sensorService.addSensor(&getHwTempSensors());
	// _sensorService.addSensor(&getHwInsideTempSensor());
	_sensorService.addSensor(&getHwMp3Sensor());

	// _swithService.addSwitch(&getHwLightSwitch());
	// _swithService.addSwitch(&getHwHeatSwitch());
	_swithService.addSwitch(&getHwRgbSwitch());
}

sensor::Mp3Sensor& project::getHwMp3Sensor()
{
	return _mp3Sensor;
}


// sensor::TemperatureSensor& project::getHwInsideTempSensor()
// {
// 	return *_sensorTemperatureInside;
// }

// sensor::TemperatureSensor& project::getHwOutsideTempSensor()
// {
// 	return *_sensorTemperatureOutside;
// }

// switches::TimeSwitch& project::getHwLightSwitch()
// {
// 	return _switchLight;
// }

// switches::HeatSwitch& project::getHwHeatSwitch()
// {
// 	return _switchHeating;
// }

// switches::FanSwitch& project::getHwFanSwitch()
// {
// 	return _switchFan;
// }

switches::RgbSwitch& project::getHwRgbSwitch()
{
	return _rgbSwitch;
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
