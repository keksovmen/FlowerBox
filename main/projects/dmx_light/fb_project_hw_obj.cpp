#include "fb_dmx_light_hw_obj.hpp"

#include <cstring>

#include "fb_dmx_light_pins.hpp"
#include "fb_globals.hpp"
#include "fb_http_puller_32.hpp"
#include "fb_keyboard_handler.hpp"

#include "esp_dmx.h"
#include "cJSON.h"



#define _MP3_UART_PORT UART_NUM_1

#define _DMX_UART_PORT UART_NUM_2

#define _DMX_TASK_STACK 4 * 1024
#define _DMX_TASK_PRIORITY 20

#define _DEFAULT_HTTP_PULLER_URL "https://gameofmind.ru/_projects/hardware_management/get.php"


using namespace fb;
using namespace project;



//function declarations
static void _httpRequestHandler(std::optional<std::string_view>);



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

	//parse json
	cJSON* idJson = cJSON_GetObjectItemCaseSensitive(json, "id");
	cJSON* valueJson = cJSON_GetObjectItemCaseSensitive(json, "value");

	if (!cJSON_IsNumber(idJson) || !cJSON_IsString(valueJson)) {
		//failure do nothing
		FB_DEBUG_LOG_E_TAG("Id is not an int and value is not a string");

		// Освобождение памяти
		cJSON_Delete(json);
		return;
	}

	const int id = idJson->valueint;
	const char* valuePtr = valueJson->valuestring;
	FB_DEBUG_LOG_I_TAG("ID=%d, value=%s", id, valuePtr);

	const std::string value = valuePtr;

	// Освобождение памяти
	cJSON_Delete(json);


	//изменение свойства
	auto* prop = global::getFlowerBox()->getProperty(id);
	if(prop){
		prop->setFromJson(value);
	}
}



static void _dmx_send_task(void* arg)
{
	FB_DEBUG_LOG_I_TAG("Started DMX send task");

	for(;;)
	{
		dmx_send(_DMX_UART_PORT);
		// uint8_t data[DMX_PACKET_SIZE];
		// dmx_packet_t packet;

		// int size = dmx_receive(_MP3_UART_PORT, &packet, DMX_TIMEOUT_TICK);
		// if(size > 0){
		// 	dmx_read(_MP3_UART_PORT, data, size);
		// 	FB_DEBUG_LOG_I_TAG("Read from DMX: err = %d, rdm = %d, sc = %d, size = %d", static_cast<int>(packet.err), packet.is_rdm, packet.sc, packet.size);
		// 	FB_DEBUG_LOG_I_TAG("Read from DMX: R=%u, G=%u, B=%u", data[1], data[2], data[3]);
		// }else{
		// 	FB_DEBUG_LOG_W_TAG("NOTHING TO RECEIVE");
		// 	vTaskDelay(pdMS_TO_TICKS(1000));
		// }
	}

	vTaskDelete(NULL);
}

void project::initHwObjs()
{
	_sensorService.addSensor(&getHwMp3Sensor());
	_sensorService.addSensor(&getHwKeyboardSensor());

	_swithService.addSwitch(&getHwRgbSwitch());

	_rgbSwitch.init();

	//register key handler for dropping WIFI settings
	global::getEventManager()->attachListener(&_keyboardHandler);

	// _rgbSwitch.setForseFlag(switches::SwitchForseState::ON);
	// _rgbSwitch.check();

	// for(;;){
	// 	for(int slot = 0; slot < 8; slot++){
	// 		FB_DEBUG_LOG_I_TAG("Writing slot %d", slot);

	// 		for(int val = 0; val < 255; val++){
	// 			dmx_write_slot(_DMX_UART_PORT, 0, 0);
	// 			dmx_write_slot(_DMX_UART_PORT, 1 + slot, val);
	// 			dmx_send(_DMX_UART_PORT);
	// 		}
	// 	}
	// }


	// // First, use the default DMX configuration...
	// dmx_config_t config = DMX_CONFIG_DEFAULT;
	
	// // ...declare the driver's DMX personalities...
	// const int personality_count = 1;
	// dmx_personality_t personalities[] = {
	// 	{1, "Default Personality"}
	// };
	
	// // ...install the DMX driver...
	// // dmx_driver_install(_DMX_UART_PORT, &config, personalities, personality_count);
	// dmx_driver_install(_MP3_UART_PORT, &config, personalities, personality_count);
	
	// // dmx_set_pin(_DMX_UART_PORT, PIN_DMX_TX, PIN_DMX_RX, PIN_DMX_RTS);
	// dmx_set_pin(_MP3_UART_PORT, PIN_MP3_TX, PIN_MP3_RX, _MP3_PIN_RTS);

	xTaskCreate(&_dmx_send_task, "DMX_READER", _DMX_TASK_STACK, NULL, _DMX_TASK_PRIORITY, NULL);

	//read it from NVS
	_httpPuller.setUrl(_DEFAULT_HTTP_PULLER_URL);
	_httpPuller.start();

	global::getEventManager()->attachListener(&_httpPuller);
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
