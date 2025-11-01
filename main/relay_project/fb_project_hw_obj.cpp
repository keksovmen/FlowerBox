#include "fb_project_hw_obj.hpp"

#include <cstring>

#include "fb_globals.hpp"
#include "fb_gpio_switch.hpp"
#include "fb_http_puller.hpp"
#include "fb_keyboard_handler.hpp"
#include "fb_pins.hpp"



#define _MP3_UART_PORT UART_NUM_1

#define _DMX_UART_PORT UART_NUM_2

#define _DMX_TASK_STACK 4 * 1024
#define _DMX_TASK_PRIORITY 20

#define _DEFAULT_HTTP_PULLER_URL "https://gameofmind.ru/_projects/hardware_management/get.php"


using namespace fb;
using namespace project;



//сенсоры туть
static sensor::KeyboardSensor<1> _keyboardSensor({std::pair{pins::PIN_KEYBOARD_RESET, h::ButtonVK::VK_0}});

// //переключатели туть
static switches::ArrayGpioSwitch<6> _gpioSwitch(
	{switches::GpioSwitch{4},
	switches::GpioSwitch{5},
	switches::GpioSwitch{6},
	switches::GpioSwitch{8},
	switches::GpioSwitch{11},
	switches::GpioSwitch{12}});

// //сервисы туть
static sensor::SensorService _sensorService;
static switches::SwitchService _swithService;

//склады туть
static sensor::SensorStorage _sensorStorage;

//прочее туть
static keyboard::KeyboardHandler _keyboardHandler;
static HttpPuller _httpPuller;



static const char* TAG = "hw";



void project::initHwObjs()
{
	_gpioSwitch.turnOffAll();

	_sensorService.addSensor(&getHwKeyboardSensor());

	_swithService.addSwitch(&_gpioSwitch);

	//register key handler for dropping WIFI settings
	global::getEventManager()->attachListener(&_keyboardHandler);

	//read it from NVS
	// _httpPuller.setUrl(_DEFAULT_HTTP_PULLER_URL);
	// _httpPuller.start();

	global::getEventManager()->attachListener(&_httpPuller);
}

sensor::KeyboardSensor<1>& project::getHwKeyboardSensor()
{
	return _keyboardSensor;
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

switches::ArrayGpioSwitch<6>& project::getHwGpioSwitch()
{
	return _gpioSwitch;
}
