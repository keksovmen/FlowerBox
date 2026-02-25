#include "fb_dmx_light_hw_obj.hpp"

#include <cstring>

#if _HW_VERSION == 2
	#include "fb_wrappers.hpp"
	#include "fb_dmx_light_pins_c3.hpp"
#else
	#include "fb_dmx_light_pins.hpp"
#endif
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



using namespace fb;
using namespace project;



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



static const char* TAG = "hw";



static void _dmx_send_task(void* arg)
{
	FB_DEBUG_LOG_I_TAG("Started DMX send task");
	vTaskDelay(pdMS_TO_TICKS(2000));

	for(;;)
	{
		// Write full zero packet first to clear receiver noise
		//better each time write data to dmx buffer due to RX interrupts pushing garbage in to the buffer
		_dmxHal.send();
		vTaskDelay(pdMS_TO_TICKS(20));
	}

	vTaskDelete(NULL);
}



static void _mqtt_data_handler(std::string_view topic, std::string_view data)
{
	FB_DEBUG_LOG_I_TAG("Data handler: %.*s -> %.*s", topic.length(), topic.cbegin(), data.length(), data.cbegin());

	if(topic == _MQTT_RGB_PATH){
		uint8_t buff[512] = {0};
		uint16_t i = 0;
		const bool result = json_util::parseNumberArray(data, "data", [&buff, &i](int val){
			if(val >= 512){
				FB_DEBUG_LOG_E_TAG("Illegal index in array! %d", i);
				return;
			}

			buff[i] = val;
			i++;
		});

		if(!result){
			FB_DEBUG_LOG_E_TAG("Failed to parse json!");
		}else{
			_dmxHal.write(0, std::span<uint8_t>(buff, i));
		}
	}else if(topic == _MQTT_RELAY_PATH){
		#if _HW_VERSION == 2
			cJSON* obj = cJSON_ParseWithLength(data.begin(), data.length());
			const int state = json_util::getIntFromJsonOrDefault(obj, "state", 0);
			cJSON_Delete(obj);

			_relayWrapper.setValue(state);
		#else

		#endif
	}else{
		FB_DEBUG_LOG_W_TAG("Unexpected MQTT topic!");
	}
}



static void _init_from_settings()
{
	_mqtt.init(settings::getIp(), settings::getPort());
	_mqtt.registerSubscribeHandler([](auto consumer){
		std::invoke(consumer, _MQTT_RGB_PATH, 2);
		std::invoke(consumer, _MQTT_RELAY_PATH, 2);
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

	_init_from_settings();

	//register key handler for dropping WIFI settings
	global::getEventManager()->attachListener(&_keyboardHandler);
	global::getEventManager()->attachListener(&_mqtt);

	xTaskCreate(&_dmx_send_task, "DMX_READER", _DMX_TASK_STACK, NULL, _DMX_TASK_PRIORITY, NULL);
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