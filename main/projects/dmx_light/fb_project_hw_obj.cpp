#include "fb_dmx_light_hw_obj.hpp"

#include <cstring>

// #include "fb_http_puller_32.hpp"
#include "fb_dmx_light_pins.hpp"
#include "fb_dmx_light_settings.hpp"
#include "fb_globals.hpp"
#include "fb_json_util.hpp"
#include "fb_keyboard_handler.hpp"

#include "esp_dmx.h"
#include "cJSON.h"



#define _MP3_UART_PORT UART_NUM_1

#define _DMX_UART_PORT UART_NUM_2

#define _DMX_TASK_STACK 4 * 1024
#define _DMX_TASK_PRIORITY 20

// #define _DEFAULT_HTTP_PULLER_URL "https://gameofmind.ru/_projects/hardware_management/get.php"
#define _MQTT_RGB_PATH ("/dmx/" + std::to_string(settings::getMqttId()) + "/rgb")


using namespace fb;
using namespace project;



//function declarations
// static void _httpRequestHandler(std::optional<std::string_view>);



//сенсоры туть
static sensor::Mp3Sensor _mp3Sensor(_MP3_UART_PORT, pins::PIN_MP3_RX, pins::PIN_MP3_TX);
static sensor::KeyboardSensor<1> _keyboardSensor({std::pair{pins::PIN_KEYBOARD_RESET, h::ButtonVK::VK_0}});

// //переключатели туть
static switches::RgbSwitchDmx _rgbSwitch(_DMX_UART_PORT, pins::PIN_DMX_RX, pins::PIN_DMX_TX, pins::PIN_DMX_RTS);
			

// //сервисы туть
static sensor::SensorService _sensorService;
static switches::SwitchService _swithService;

//склады туть
static sensor::SensorStorage _sensorStorage;

//прочее туть
static keyboard::KeyboardHandler _keyboardHandler;
// static HttpPuller _httpPuller(&_httpRequestHandler);
static periph::MqttClient _mqtt;



static const char* TAG = "hw";



// static void _httpRequestHandler(std::optional<std::string_view> data)
// {
// 	//failure case
// 	if(!data){
// 		return;
// 	}

// 	// Парсинг JSON
// 	cJSON* json = cJSON_Parse(data->cbegin());
// 	if(!json){
// 		FB_DEBUG_LOG_E_TAG("Failed to parse JSON");
// 		return;
// 	}

// 	//parse json
// 	cJSON* idJson = cJSON_GetObjectItemCaseSensitive(json, "id");
// 	cJSON* valueJson = cJSON_GetObjectItemCaseSensitive(json, "value");

// 	if (!cJSON_IsNumber(idJson) || !cJSON_IsString(valueJson)) {
// 		//failure do nothing
// 		FB_DEBUG_LOG_E_TAG("Id is not an int and value is not a string");

// 		// Освобождение памяти
// 		cJSON_Delete(json);
// 		return;
// 	}

// 	const int id = idJson->valueint;
// 	const char* valuePtr = valueJson->valuestring;
// 	FB_DEBUG_LOG_I_TAG("ID=%d, value=%s", id, valuePtr);

// 	const std::string value = valuePtr;

// 	// Освобождение памяти
// 	cJSON_Delete(json);


// 	//изменение свойства
// 	auto* prop = global::getFlowerBox()->getProperty(id);
// 	if(prop){
// 		prop->setFromJson(value);
// 	}
// }



static void _dmx_send_task(void* arg)
{
	FB_DEBUG_LOG_I_TAG("Started DMX send task");
	vTaskDelay(pdMS_TO_TICKS(2000));
	// Write full zero packet first to clear receiver noise
	uint8_t zero_packet[DMX_PACKET_SIZE_MAX] = {0};
	dmx_write(_DMX_UART_PORT, zero_packet, sizeof(zero_packet));

	for(;;)
	{
		//better each time write data to dmx buffer due to RX interrupts pushing garbage in to the buffer
		_rgbSwitch.writeDmx();
		dmx_send(_DMX_UART_PORT);
	}

	vTaskDelete(NULL);
}



static void _mqtt_data_handler(std::string_view topic, std::string_view data)
{
	FB_DEBUG_LOG_I_TAG("Data handler: %.*s -> %.*s", topic.length(), topic.cbegin(), data.length(), data.cbegin());

	if(topic == _MQTT_RGB_PATH){
		auto vals = json_util::parseRgbFromJson(data);
		_rgbSwitch.setColor(vals.r, vals.g, vals.b);
	}else{
		FB_DEBUG_LOG_W_TAG("Unexpected MQTT topic!");
	}
}



static void _init_from_settings()
{
	_mqtt.init(settings::getIp(), settings::getPort());
	_mqtt.registerSubscribeHandler([](auto consumer){
		std::invoke(consumer, _MQTT_RGB_PATH, 2);
	});
	_mqtt.addDataHandler(&_mqtt_data_handler);
}

void project::initHwObjs()
{
	_sensorService.addSensor(&getHwMp3Sensor());
	_sensorService.addSensor(&getHwKeyboardSensor());

	_swithService.addSwitch(&getHwRgbSwitch());

	_rgbSwitch.setForseFlag(switches::SwitchForseState::ON);
	_rgbSwitch.init();

	_init_from_settings();

	//register key handler for dropping WIFI settings
	global::getEventManager()->attachListener(&_keyboardHandler);
	global::getEventManager()->attachListener(&_mqtt);

	xTaskCreate(&_dmx_send_task, "DMX_READER", _DMX_TASK_STACK, NULL, _DMX_TASK_PRIORITY, NULL);

	//read it from NVS
	// _httpPuller.setUrl(_DEFAULT_HTTP_PULLER_URL);
	// _httpPuller.start();

	// global::getEventManager()->attachListener(&_httpPuller);
}

sensor::Mp3Sensor& project::getHwMp3Sensor()
{
	return _mp3Sensor;
}

sensor::KeyboardSensor<1>& project::getHwKeyboardSensor()
{
	return _keyboardSensor;
}

switches::RgbSwitchDmx& project::getHwRgbSwitch()
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

periph::MqttClient& project::getHwMqttClient()
{
	return _mqtt;
}