#include "fb_relay_hw_obj.hpp"

#include <cstring>

#include "fb_ac_dimmer.hpp"
#include "fb_core.hpp"
#include "fb_globals.hpp"
#include "fb_gpio_switch.hpp"
#include "fb_http_puller_32.hpp"
#include "fb_json_util.hpp"
#include "fb_keyboard_handler.hpp"
#include "fb_mqtt_client.hpp"
#include "fb_relay_settings.hpp"
#include "fb_wrappers.hpp"

#include "cJSON.h"



#define _MQTT_PATH_MODE ("/relay/" + std::to_string(settings::getMqttId()) + "/state")



using namespace fb;
using namespace project;



//function declarations
static void _httpRequestHandler(std::optional<std::string_view>);



static const char* TAG = "hw";



//сенсоры туть
static sensor::KeyboardSensor<1> _keyboardSensor({std::pair{pins::PIN_KEYBOARD_RESET, h::ButtonVK::VK_0}});

// //переключатели туть
#if _HW_VERSION == 2
	static switches::ArrayGpioSwitch<pins::RELAY_PINS.size()> _gpioSwitch(pins::RELAY_PINS, false);
#else
	static switches::ArrayGpioSwitch<pins::RELAY_PINS.size()> _gpioSwitch(pins::RELAY_PINS, true);
#endif

// //сервисы туть
static sensor::SensorService _sensorService;
static switches::SwitchService _swithService;

//склады туть
static sensor::SensorStorage _sensorStorage;

//прочее туть
static keyboard::KeyboardHandler _keyboardHandler;
static HttpPuller _httpPuller(&_httpRequestHandler);
static periph::MqttClient _mqtt;
static fb::util::AcDimmer<pins::RELAY_PINS.size()> _dimmer([](int index, bool state){
	if(state){
		_gpioSwitch.turnOn(index);
	}else{
		_gpioSwitch.turnOff(index);
	}
});



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

	std::array<bool, pins::RELAY_PINS.size()> result{};

	for(auto i = 0; i < result.size(); i++){
		//parse json
		const std::string id = "light" + std::to_string(i + 1);	//+1 начинаем считать с 1
		cJSON* valueJson = cJSON_GetObjectItemCaseSensitive(json, id.c_str());

		if (!cJSON_IsNumber(valueJson)) {
			//failure do nothing
			FB_DEBUG_LOG_E_TAG("Pin %d, Value is not an int", i);

			continue;
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
			_gpioSwitch.turnOn(i);
		}else{
			_gpioSwitch.turnOff(i);
		}
	}
}



static void _handleMqtt(std::string_view topic, std::string_view data)
{
	if(topic == _MQTT_PATH_MODE){
		fb::json_util::parseObjArray(data, "data", [](cJSON* root){
			const int index = fb::json_util::getIntFromJsonOrDefault(root, "i", -1);
			if(index < 0 || index >= pins::RELAY_PINS.size()){
				FB_DEBUG_LOG_W_TAG("Illegal index %d", index);
				return;
			}

			const int mode = fb::json_util::getIntFromJsonOrDefault(root, "mode", -1);
			if(mode < 0 || mode >= (int) decltype(_dimmer)::Mode::MAX){
				FB_DEBUG_LOG_W_TAG("Illegal mode %d", mode);
				return;
			}

			const int minTick = fb::json_util::getIntFromJsonOrDefault(root, "min", -1);
			const int maxTick = fb::json_util::getIntFromJsonOrDefault(root, "max", -1);

			if(minTick >= 0 && maxTick > 0){
				_dimmer.setBlinkPeriod(index, std::min(minTick, maxTick), std::max(maxTick, minTick));
			}

			_dimmer.setState(index, (decltype(_dimmer)::Mode) mode);
		});
	}
}



void project::initHwObjs()
{
	_sensorService.addSensor(&getHwKeyboardSensor());

	_gpioSwitch.turnOffAll();
	_swithService.addSwitch(&_gpioSwitch);

	//register key handler for dropping WIFI settings
	global::getEventManager()->attachListener(&_keyboardHandler);

	//read it from NVS
#if _HW_VERSION == 2
	_mqtt.init(settings::getIp(), settings::getPort(), 4 * 1024);
	_mqtt.addDataHandler(&_handleMqtt);
	_mqtt.registerSubscribeHandler([](const auto& handler){
		std::invoke(handler, _MQTT_PATH_MODE, 2);
	});
	global::getEventManager()->attachListener(&_mqtt);

	_dimmer.startTask(10, 4 * 1024);
#else
	_httpPuller.setUrl(settings::getHttpUrl());
	_httpPuller.setTimeoutMs(settings::getHttpDelay());
	_httpPuller.start();
	global::getEventManager()->attachListener(&_httpPuller);
#endif
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

switches::ArrayGpioSwitch<pins::RELAY_PINS.size()>& project::getHwGpioSwitch()
{
	return _gpioSwitch;
}

AbstractHttpPuller& project::getHwHttpPuller()
{
	return _httpPuller;
}