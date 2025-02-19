#include "fb_project_hw_obj.hpp"

#include "fb_pins.hpp"

#include "esp_dmx.h"



#define _MP3_PIN_TX 18
#define _MP3_PIN_RX 19
#define _MP3_UART_PORT UART_NUM_1

#define _DMX_TX 22
#define _DMX_RX 23
#define _DMX_RTS 21
#define _DMX_UART_PORT UART_NUM_2




using namespace fb;
using namespace project;



//сенсоры туть
static sensor::Mp3Sensor _mp3Sensor(_MP3_UART_PORT, _MP3_PIN_RX, _MP3_PIN_TX);

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

	// First, use the default DMX configuration...
	dmx_config_t config = DMX_CONFIG_DEFAULT;
	
	// ...declare the driver's DMX personalities...
	const int personality_count = 1;
	dmx_personality_t personalities[] = {
		{1, "Default Personality"}
	};
	
	// ...install the DMX driver...
	dmx_driver_install(_DMX_UART_PORT, &config, personalities, personality_count);
	
	dmx_set_pin(_DMX_UART_PORT, _DMX_TX, _DMX_RX, _DMX_RTS);
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
