#include "fb_project_hw_obj.hpp"

#include "fb_pins.hpp"



#define _MP3_PIN_RX 16
#define _MP3_PIN_TX 17



using namespace fb;
using namespace project;



//сенсоры туть
static sensor::Mp3Sensor _mp3Sensor(_MP3_PIN_RX, _MP3_PIN_TX);

// //переключатели туть
static switches::RgbSwitch _rgbSwitch(LEDC_TIMER_0, LEDC_CHANNEL_0,
			pins::PIN_RED_LED, pins::PIN_GREEN_LED, pins::PIN_BLUE_LED);
			

// //сервисы туть
static sensor::SensorService _sensorService;
static switches::SwitchService _swithService;

//склады туть
static sensor::SensorStorage _sensorStorage;



void project::initHwObjs()
{
	_sensorService.addSensor(&getHwMp3Sensor());

	_swithService.addSwitch(&getHwRgbSwitch());
}

sensor::Mp3Sensor& project::getHwMp3Sensor()
{
	return _mp3Sensor;
}

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
