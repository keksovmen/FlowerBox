#include "fb_dmx_light_hw_obj.hpp"

#include <cstring>

#if _HW_VERSION == 2
	#include "fb_wrappers.hpp"
	#include "fb_dmx_light_pins_c3.hpp"
#else
	#include "fb_dmx_light_pins.hpp"
#endif
#include "fb_channel_handler.hpp"
#include "fb_dmx_light_settings.hpp"
#include "fb_globals.hpp"
#include "fb_json_util.hpp"
#include "fb_keyboard_handler.hpp"

#include "cJSON.h"
#include "hal/uart_types.h"



#if _HW_VERSION == 2
	#define _DMX_UART_PORT UART_NUM_1
#else
	#define _MP3_UART_PORT UART_NUM_1
	#define _DMX_UART_PORT UART_NUM_2
#endif

#define _DMX_TASK_STACK 4 * 1024
#define _DMX_TASK_PRIORITY 20

#define _MQTT_RGB_PATH ("/dmx/" + std::to_string(settings::getMqttId()) + "/rgb")
#define _MQTT_RELAY_PATH ("/dmx/" + std::to_string(settings::getMqttId()) + "/relay")
#define _MQTT_FUNC_PATH ("/dmx/" + std::to_string(settings::getMqttId()) + "/func")
#define _MQTT_SCENERY_PATH ("/dmx/" + std::to_string(settings::getMqttId()) + "/scenery")
#define _MQTT_ERROR_PATH ("/dmx/" + std::to_string(settings::getMqttId()) + "/err")
#define _MQTT_CLEAR_PATH ("/dmx/" + std::to_string(settings::getMqttId()) + "/clear")

#define _DMX_TICK_RATE_HZ 50



using namespace fb;
using namespace project;



static const char* TAG = "hw";



//сенсоры туть
#if _HW_VERSION == 2
	static wrappers::WrapperGpio _relayWrapper(static_cast<gpio_num_t>(pins::PIN_RELAY), false);
#else
	static sensor::Mp3Sensor _mp3Sensor(_MP3_UART_PORT, pins::PIN_MP3_RX, pins::PIN_MP3_TX);
#endif
static sensor::KeyboardSensor<1> _keyboardSensor({std::pair{pins::PIN_KEYBOARD_RESET, h::ButtonVK::VK_0}});

// //переключатели туть
static periph::DmxHal _dmxHal;

// //сервисы туть
static sensor::SensorService _sensorService;
static switches::SwitchService _swithService;

//склады туть
static sensor::SensorStorage _sensorStorage;

//прочее туть
static keyboard::KeyboardHandler _keyboardHandler;
static periph::MqttClient _mqtt;
static fb::util::SceneryHandler _logicHandler([](uint16_t channel, uint8_t val){_dmxHal.write(channel, val);});



static void _dmx_send_task(void* arg)
{
	FB_DEBUG_LOG_I_TAG("Started DMX send task");
	vTaskDelay(pdMS_TO_TICKS(2000));

	for(;;)
	{
		// Write full zero packet first to clear receiver noise
		//better each time write data to dmx buffer due to RX interrupts pushing garbage in to the buffer
		_logicHandler.tick();
		_dmxHal.send();
		vTaskDelay(pdMS_TO_TICKS(1000 / _DMX_TICK_RATE_HZ));
	}

	vTaskDelete(NULL);
}


static bool _checkBoundaries(int val, int minVal, int maxVal, const char* msg)
{
	if(val < minVal || val > maxVal){
		FB_DEBUG_LOG_E_TAG("%s: %d [%d/%d]", msg, val, minVal, maxVal);
		return false;
	}

	return true;
}

static void _handleRgbTopic(std::string_view topic, std::string_view data)
{
	uint16_t i = 0;
	const bool result = json_util::parseNumberArray(data, "data", [&i](int val){
		if(!_checkBoundaries(i, 0, 511, "Illegal index in array") ||
			!_checkBoundaries(val, 0, 255, "Illegal value in array"))
		{
			return;
		}

		_dmxHal.write(i, val);
		i++;
	});

	if(!result){
		FB_DEBUG_LOG_E_TAG("Failed to parse json!");
	}
}

static void _handleRelayTopic(std::string_view topic, std::string_view data)
{
	#if _HW_VERSION == 2
		cJSON* obj = cJSON_ParseWithLength(data.begin(), data.length());
		const int state = json_util::getIntFromJsonOrDefault(obj, "state", 0);
		cJSON_Delete(obj);

		_relayWrapper.setValue(state);
	#else

	#endif
}

static void _handleFunctionTopic(std::string_view topic, std::string_view data)
{
	json_util::parseObjArray(data, "data", [](cJSON* obj){
		const uint16_t channel = json_util::getIntFromJsonOrDefault(obj, "i", 0);
		const int val = json_util::getIntFromJsonOrDefault(obj, "v", -1);
		const uint8_t valMin = json_util::getIntFromJsonOrDefault(obj, "a", 0);
		const uint8_t valMax = json_util::getIntFromJsonOrDefault(obj, "b", 0);
		const uint32_t periodMs = json_util::getIntFromJsonOrDefault(obj, "T", 0);
		const bool cycle = json_util::getIntFromJsonOrDefault(obj, "cycle", 0);

		if(val == -1){
			if(cycle){
				_logicHandler.addScenery(channel, {
						{valMin, 0},
						{valMax, periodMs},
						{valMin, periodMs}
					}, {{2, 0, fb::util::RepeatEntry::INFINITE}});
			}else{
				_logicHandler.addScenery(channel, {{valMin, 0}, {valMax, periodMs}}, {});
			}
		}else{
			_dmxHal.write(channel, val);
		}
	});
}


static std::vector<uint32_t> _parseIntWithBoundary(cJSON* channelObj, std::string_view key, uint32_t minVal, uint32_t maxVal)
{
	std::vector<uint32_t> vals;
	const bool result = json_util::parseNumberArray(channelObj, key, [&vals, minVal, maxVal](int val){
		//check length we only support 255
		if(!_checkBoundaries(vals.size(), 0, 255, "Array overflow!") ||
			!_checkBoundaries(val, minVal, maxVal, "Illegal value in array!"))
		{
			return;
		}

		vals.push_back(static_cast<uint32_t>(val));
	});

	return result ? vals : std::vector<uint32_t>{};
}

static std::vector<uint32_t> _parseChannels(cJSON* channelObj)
{
	return _parseIntWithBoundary(channelObj, "i", 0, 511);
}

static std::vector<uint32_t> _parseVals(cJSON* channelObj)
{
	return _parseIntWithBoundary(channelObj, "vals", 0, 255);
}

static std::vector<uint32_t> _parseTimings(cJSON* channelObj)
{
	return _parseIntWithBoundary(channelObj, "times", 0, ((1 << 16) - 1) * fb::util::SceneryHandler::TICK_VALUE_MS);
}

static std::optional<std::vector<fb::util::RepeatEntry>> _parseRepeat(cJSON* channelObj, int valsLength)
{
	std::vector<fb::util::RepeatEntry> repeatEntries;

	const bool result = json_util::parseObjArray(channelObj, "repeat", [&repeatEntries, &valsLength](cJSON* repeatObj){
		const int from = json_util::getIntFromJsonOrDefault(repeatObj, "f", -1);
		const int to = json_util::getIntFromJsonOrDefault(repeatObj, "t", -1);
		const int count = json_util::getIntFromJsonOrDefault(repeatObj, "c", -1);

		//check lengths we only support 255
		if(!_checkBoundaries(from, 0, valsLength - 1, "Illegal FROM!") ||
			!_checkBoundaries(to, 0, valsLength - 2, "Illegal TO!") ||
			!_checkBoundaries(count, -1, 0x7FFF, "Illegal COUNT!"))
		{
			FB_DEBUG_LOG_E_TAG("Illegal FROM[%d] or TO[%d], length = %d, count = %d!", from, to, valsLength, count);
			return;
		}

		repeatEntries.push_back({static_cast<uint8_t>(from), static_cast<uint8_t>(to), static_cast<int16_t>(count)});
	});

	//repeat could be not specified it is not an error, only error when it is not empty, but still not 100% true
	if(!result && !repeatEntries.empty()){
		return {};
	}else{
		return repeatEntries;
	}
}

static void _handleSceneryTopic(std::string_view topic, std::string_view data)
{
	const bool result = json_util::parseObjArray(data, "data", [](cJSON* channelObj){
		//parse channel indexes array
		std::vector<uint32_t> channels = _parseChannels(channelObj);
		if(channels.empty()){
			_mqtt.publish(_MQTT_ERROR_PATH, "Illegal or bad json in \"i\"");
			FB_DEBUG_LOG_E_TAG("Failed to parse channels json or they empty!");
			return;
		}

		//first put values in to base entries
		std::vector<uint32_t> vals = _parseVals(channelObj);
		if(vals.empty()){
			_mqtt.publish(_MQTT_ERROR_PATH, "Illegal value in \"vals\"");
			FB_DEBUG_LOG_E_TAG("Failed to parse values json!");
			return;
		}

		//then put time for base entries
		std::vector<uint32_t> timings = _parseTimings(channelObj);
		if(timings.empty()){
			_mqtt.publish(_MQTT_ERROR_PATH, "Illegal value in \"times\"");
			FB_DEBUG_LOG_E_TAG("Failed to parse times json!");
			return;
		}

		//check we must be equal otherwise there is an error
		if(vals.size() != timings.size()){
			FB_DEBUG_LOG_E_TAG("Not equal vals and times arrays: %d != %d!", vals.size(), timings.size());
			_mqtt.publish(_MQTT_ERROR_PATH, "Length of \"vals\" and \"times\" are not equal");
			return;
		}

		//now parse repeat section
		auto optionalRepeat = _parseRepeat(channelObj, vals.size());
		if(!optionalRepeat){
			_mqtt.publish(_MQTT_ERROR_PATH, "Illegal indexes or bad json in \"repeat\"");
			FB_DEBUG_LOG_E_TAG("Failed to parse repeat json!");
			return;
		}

		//reserve memory for base entries
		std::vector<fb::util::BaseEntry> baseEntries;
		baseEntries.reserve(vals.size());

		//do not like having two arrays that already could be result,
		//but for better readability, lets use more memory
		//create BaseEntry array
		std::transform(vals.begin(), vals.end(), timings.begin(), std::back_inserter(baseEntries),
			[](uint32_t val, uint32_t tim){return fb::util::BaseEntry{static_cast<uint8_t>(val), tim};});

		//BEWARE it could have small delay because DMX task is runing constantly and there is no global lock
		//and we putting sceneries in sequence not all at one time, so maybe add dmx task hold time here
		//or just ignore it only delayed by one or two ticks 20-40 ms, for 30 channels json, if disable log there should be
		//even less delay
		//and finally add scenery
		for(int i = 0; i < channels.size() - 1; i++){
			_logicHandler.addScenery(static_cast<uint16_t>(channels[i]), baseEntries, optionalRepeat.value());
		}
		//do not copy the last channel it is dangerous due to memory constrains
		_logicHandler.addScenery(static_cast<uint16_t>(channels.back()), std::move(baseEntries), std::move(optionalRepeat.value()));
	});

	if(!result){
		FB_DEBUG_LOG_E_TAG("Failed to handle request!");
		return;
	}
}

static void _handleClearTopic(std::string_view topic, std::string_view data)
{
	_logicHandler.clearAll();

	uint8_t buff[512] = {0};
	_dmxHal.write(0, buff);
}

static void _mqtt_data_handler(std::string_view topic, std::string_view data)
{
	if(topic == _MQTT_RGB_PATH){
		_handleRgbTopic(topic, data);

	}else if(topic == _MQTT_RELAY_PATH){
		_handleRelayTopic(topic, data);

	}else if(topic == _MQTT_FUNC_PATH){
		_handleFunctionTopic(topic, data);

	}else if(topic == _MQTT_SCENERY_PATH){
		_handleSceneryTopic(topic, data);

	}else if(topic == _MQTT_CLEAR_PATH){
		_handleClearTopic(topic, data);

	}else{
		FB_DEBUG_LOG_W_TAG("Unexpected MQTT topic!");
	}
}



static void _init_from_settings()
{
	_mqtt.init(settings::getIp(), settings::getPort(), 4 * 1024);
	_mqtt.registerSubscribeHandler([](auto consumer){
		std::invoke(consumer, _MQTT_RGB_PATH, 2);
		std::invoke(consumer, _MQTT_RELAY_PATH, 2);
		std::invoke(consumer, _MQTT_FUNC_PATH, 2);
		std::invoke(consumer, _MQTT_SCENERY_PATH, 2);
		std::invoke(consumer, _MQTT_CLEAR_PATH, 2);
	});
	_mqtt.addDataHandler(&_mqtt_data_handler);
}

void project::initHwObjs()
{
	#if _HW_VERSION == 2
		_relayWrapper.init();
	#else
		_sensorService.addSensor(&getHwMp3Sensor());
	#endif

	_sensorService.addSensor(&getHwKeyboardSensor());

	_dmxHal.init(_DMX_UART_PORT, pins::PIN_DMX_RX, pins::PIN_DMX_TX, pins::PIN_DMX_RTS);
	_logicHandler.init();

	_init_from_settings();


	//register key handler for dropping WIFI settings
	global::getEventManager()->attachListener(&_keyboardHandler);
	global::getEventManager()->attachListener(&_mqtt);

	xTaskCreate(&_dmx_send_task, "DMX_HANDLER", _DMX_TASK_STACK, NULL, _DMX_TASK_PRIORITY, NULL);
}

#if _HW_VERSION == 2
	wrappers::WrapperGpio& project::getHwRelay()
	{
		return _relayWrapper;
	}
#else
	sensor::Mp3Sensor& project::getHwMp3Sensor()
	{
		return _mp3Sensor;
	}
#endif

sensor::KeyboardSensor<1>& project::getHwKeyboardSensor()
{
	return _keyboardSensor;
}

periph::DmxHal& project::getHwDmxHal()
{
	return _dmxHal;
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

periph::MqttClient& project::getHwMqttClient()
{
	return _mqtt;
}