#include "fb_audio_multi_hw_obj.hpp"

#include "fb_globals.hpp"
#include "fb_keyboard_handler.hpp"
#include "fb_audio_multi_pins.hpp"
#include "fb_audio_multi_settings.hpp"
#include "fb_sensor_mp3.hpp"
#include "fb_mqtt_client.hpp"
#include "fb_json_util.hpp"

#include "driver/gpio.h"
#include "cJSON.h"



#define _MP3_UART_PORT UART_NUM_1

#define _MQTT_PLAY_PATH ("/audio_multi/" + std::to_string(settings::getMqttId()) + "/play")
#define _MQTT_STOP_PATH ("/audio_multi/" + std::to_string(settings::getMqttId()) + "/stop")
#define _MQTT_VOLUME_PATH ("/audio_multi/" + std::to_string(settings::getMqttId()) + "/volume")



using namespace fb;
using namespace project;



static const char* TAG = "HW";
static const bool _CHANNEL_MAP[][(sizeof(pins::PINS_MULTIPLEX) / sizeof(pins::PINS_MULTIPLEX[0]))] = {
	{true, false, false},
	{false, false, false},
	{false, true, true},
	{false, true, false}
};


static sensor::SensorService _sensorService;
static switches::SwitchService _swithService;

static sensor::SensorStorage _sensorStorage;

static keyboard::KeyboardHandler _keyboardHandler;

static sensor::Mp3Sensor _mp3Sensor(_MP3_UART_PORT, pins::PIN_MP3_RX, pins::PIN_MP3_TX);
static periph::MqttClient _mqtt;



static void _enableChannel(int channelId)
{
	if(channelId < 0 || channelId > (sizeof(pins::PINS_MULTIPLEX) / sizeof(pins::PINS_MULTIPLEX[0]))){
		FB_DEBUG_LOG_E_TAG("Illegal channel ID! %d", channelId);
		return;
	}

	for(int i = 0; i < (sizeof(pins::PINS_MULTIPLEX) / sizeof(pins::PINS_MULTIPLEX[0])); i++){
		gpio_set_level(static_cast<gpio_num_t>(pins::PINS_MULTIPLEX[i]), _CHANNEL_MAP[channelId][i]);
	}

	FB_DEBUG_LOG_I_TAG("Enabled channel: %d", channelId);
}

static void _mqtt_data_handler(std::string_view topic, std::string_view data)
{
	if(topic == _MQTT_PLAY_PATH){
		auto* rootJson = cJSON_ParseWithLength(data.begin(), data.size());

		const int trackId = json_util::getIntFromJsonOrDefault(rootJson, "track", -1);
		const int channelId = json_util::getIntFromJsonOrDefault(rootJson, "channel", -1);

		cJSON_free(rootJson);

		if(trackId == -1 || channelId == -1){
			FB_DEBUG_LOG_E_TAG("Missing track or channel keys in json! %.*s", data.size(), data.cbegin());
			return;
		}


		_mp3Sensor.setVolume(settings::getVolume());
		_enableChannel(channelId);
		_mp3Sensor.play(trackId);

	}else if(topic == _MQTT_STOP_PATH){
		_mp3Sensor.stop();
	
	}else if(topic == _MQTT_VOLUME_PATH){
		const int volume = json_util::parseIntFromJsonOrDefault(data, "volume", -1);
		if(volume == -1){
			FB_DEBUG_LOG_E_TAG("Missing volume key in json! %.*s", data.size(), data.cbegin());
			return;
		}

		if(!_mp3Sensor.setVolume(volume)){
			FB_DEBUG_LOG_E_TAG("Illegal volume value! %d", volume);
			return;
		}

		settings::setVolume(volume);
	}
}



static void _init_from_settings()
{
	_mqtt.init(settings::getIp(), settings::getPort(), 4 * 1024);
	_mqtt.registerSubscribeHandler([](auto consumer){
		std::invoke(consumer, _MQTT_PLAY_PATH, 2);
		std::invoke(consumer, _MQTT_STOP_PATH, 2);
		std::invoke(consumer, _MQTT_VOLUME_PATH, 2);
	});
	_mqtt.addDataHandler(&_mqtt_data_handler);
}



void project::initHwObjs()
{
	_sensorService.addSensor(&_mp3Sensor);

	for(int pin : pins::PINS_MULTIPLEX){
		gpio_config_t cfg = {
			.pin_bit_mask = 1llu << pin,
			.mode = GPIO_MODE_OUTPUT,
			.pull_up_en = GPIO_PULLUP_DISABLE,
			.pull_down_en = GPIO_PULLDOWN_DISABLE,
			.intr_type = GPIO_INTR_DISABLE,
		};
		gpio_config(&cfg);
		gpio_set_level(static_cast<gpio_num_t>(pin), 0);
	}

	_init_from_settings();

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