#include "fb_keypad_backlight_hw_obj.hpp"

#include "fb_globals.hpp"
#include "fb_json_util.hpp"
#include "fb_keyboard_handler.hpp"
#include "fb_keypad_backlight_pins.hpp"
#include "fb_keypad_backlight_settings.hpp"
#include "fb_sensor_keyboard.hpp"
#include "fb_mqtt_client.hpp"



#define _MQTT_PATH_PORT ("/keyboard/" + std::to_string(settings::getMqttId()) + "/port")



using namespace fb;
using namespace project;



static const char* TAG = "HW";



static sensor::SensorService _sensorService;
static switches::SwitchService _swithService;

static sensor::SensorStorage _sensorStorage;

static keyboard::KeyboardHandler _keyboardHandler;

static sensor::KeyboardSensor<1> _keyboardSensor({std::pair{pins::PIN_KEYBOARD_RESET, h::ButtonVK::VK_0}});
static wrappers::WrapperDb135 _db135(pins::PIN_SCL, pins::PIN_MOSI, pins::PIN_SC);
static periph::MqttClient _mqtt;



static void _init_from_settings()
{

}



static void _handlePortTopic(std::string_view data)
{
	auto* json = cJSON_ParseWithLength(data.cbegin(), data.length());
	auto val = json_util::getIntFromJsonOrDefault(json, "val", 0);
	cJSON_Delete(json);

	_db135.setValue(val);
	FB_DEBUG_LOG_I_TAG("Set DB135 to %d", val);
}

static void _dataHandler(std::string_view topic, std::string_view data)
{
	//parse json data and see if you must fire
	FB_DEBUG_LOG_I_TAG("Data handler: %.*s -> %.*s", topic.length(), topic.cbegin(), data.length(), data.cbegin());

	if(topic == _MQTT_PATH_PORT){
		_handlePortTopic(data);
	}else{
		FB_DEBUG_LOG_W_TAG("Unexpected MQTT topic!");
	}
}



void project::initHwObjs()
{
	_sensorService.addSensor(&_keyboardSensor);

	_init_from_settings();

	_db135.init();

	_mqtt.init(settings::getIp(), settings::getPort());
	_mqtt.addDataHandler(&_dataHandler);
	_mqtt.registerSubscribeHandler([](const auto& handler){
		std::invoke(handler, _MQTT_PATH_PORT, 2);
	});

	//register key handler for dropping WIFI settings
	global::getEventManager()->attachListener(&_keyboardHandler);
	global::getEventManager()->attachListener(&_mqtt);
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

wrappers::WrapperDb135& project::getHwWrapperDb()
{
	return _db135;
}