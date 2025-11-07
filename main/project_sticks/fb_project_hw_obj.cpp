#include "fb_project_hw_obj.hpp"

#include <cstring>

#include "fb_globals.hpp"
#include "fb_http_puller.hpp"
#include "fb_keyboard_handler.hpp"
#include "fb_pins.hpp"
#include "fb_project_settings.hpp"
#include "fb_http_puller_32.hpp"

#include "cJSON.h"




using namespace fb;
using namespace project;



//function declarations
static void _httpRequestHandler(std::optional<std::string_view>);



//сенсоры туть
static sensor::KeyboardSensor<1> _keyboardSensor({std::pair{pins::PIN_KEYBOARD_RESET, h::ButtonVK::VK_0}});

// //сервисы туть
static sensor::SensorService _sensorService;
static switches::SwitchService _swithService;

//склады туть
static sensor::SensorStorage _sensorStorage;

//прочее туть
static keyboard::KeyboardHandler _keyboardHandler;
static HttpPuller _httpPuller(&_httpRequestHandler);



static const char* TAG = "hw";



static void _httpRequestHandler(std::optional<std::string_view> data)
{
	//failure case
	if(!data){
		return;
	}

	// Парсинг JSON
	cJSON* json = cJSON_Parse(data->cbegin());
	if(!json){
		FB_DEBUG_LOG_E_TAG("Failed to parse JSON");
		return;
	}

	std::array<bool, pins::RELAY_PINS_COUNT> result;

	for(auto i = 0; i < result.size(); i++){
		//parse json
		const std::string id = "light" + std::to_string(i + 1);	//+1 начинаем считать с 1
		cJSON* valueJson = cJSON_GetObjectItemCaseSensitive(json, id.c_str());

		if (!cJSON_IsNumber(valueJson)) {
			//failure do nothing
			FB_DEBUG_LOG_E_TAG("Pin %d, Value is not an int", i);

			// Освобождение памяти
			cJSON_Delete(json);

			return;
		}

		const int state = valueJson->valueint;
		FB_DEBUG_LOG_I_TAG("Pin: %d = Value: %d", i, state);

		result[i] = static_cast<bool>(state);
	}


	// Освобождение памяти
	cJSON_Delete(json);

	//изменение свойства
	for(auto i = 0; i < result.size(); i++){
		if(result[i]){
			// _gpioSwitch.turnOn(i);
		}else{
			// _gpioSwitch.turnOff(i);
		}
	}
}



void project::initHwObjs()
{
	_sensorService.addSensor(&getHwKeyboardSensor());

	// _gpioSwitch.turnOffAll();
	// _swithService.addSwitch(&_gpioSwitch);

	//register key handler for dropping WIFI settings
	global::getEventManager()->attachListener(&_keyboardHandler);

	//read it from NVS
	_httpPuller.setUrl(settings::getHttpUrl());
	_httpPuller.setTimeoutMs(settings::getHttpDelay());
	_httpPuller.start();
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

AbstractHttpPuller& project::getHwHttpPuller()
{
	return _httpPuller;
}