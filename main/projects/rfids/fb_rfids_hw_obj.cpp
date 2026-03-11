#include "fb_rfids_hw_obj.hpp"

#include "fb_globals.hpp"
#include "fb_http_request.hpp"
#include "fb_keyboard_handler.hpp"
#include "fb_rfids_pins.hpp"
#include "fb_rfids_settings.hpp"
#include "fb_sensor_keyboard.hpp"
#include "fb_uart.hpp"



using namespace fb;
using namespace project;



static const char* TAG = "HW";



static sensor::SensorService _sensorService;
static switches::SwitchService _swithService;

static sensor::SensorStorage _sensorStorage;

static keyboard::KeyboardHandler _keyboardHandler;

static sensor::KeyboardSensor<1> _keyboardSensor({std::pair{pins::PIN_KEYBOARD_RESET, h::ButtonVK::VK_0}});
static interfaces::Uart* _uart_0 = nullptr;
static interfaces::Uart _uart_1(UART_NUM_1, pins::RX_PINS[0], pins::TX_PIN, 9600);
static periph::HttpRequest _http;

static std::array<uint64_t, pins::RX_PINS_SIZE> _states{0};
static TaskHandle_t _senderHndl = nullptr;



static void _sendState(const std::array<uint64_t, pins::RX_PINS_SIZE>& _states)
{
	//post current state
	char buffer[256] = "[";
	char* ptr = &buffer[1];
	for(auto v : _states){
		ptr = ptr + sprintf(ptr, "%llu,", v);
	}
	ptr--;
	ptr[0] = ']';
	ptr[1] = '\0';

	FB_DEBUG_LOG_I_TAG("%s", buffer);

	//change to dedicated task that will send updates once per http send status
	_http.post(settings::getUrl(), buffer);
}

static void _sendTask(void* arg)
{
	for(;;){
		ulTaskNotifyTake(pdTRUE, portMAX_DELAY);
		_sendState(_states);
	}

	vTaskDelete(NULL);
}

uint64_t _hexstrToVal(const char *str, unsigned int length)
{
	return std::stoll(std::string(str, length), nullptr, 16); // strtol converts a null-terminated string to a long value
}

static int _findPackage(std::span<uint8_t> data)
{
	for(int i = 0; i < data.size() - 13; i++){
		if(data[i] == 2 && data[i + 13] == 3){
			//found
			return i;			
		}
	}

	return -1;
}

static void _taskReader(void* arg)
{
	std::vector<decltype(&_uart_1)> readers;
	if(_uart_0 != nullptr){
		readers.push_back(_uart_0);
	}

	readers.push_back(&_uart_1);

	const int periodMs = settings::getReadTimeMs();

	for(;;){
		bool changed = false;

		for(int i = 0; i < pins::RX_PINS_SIZE; i += readers.size()){
			const bool lastFlag = i == (pins::RX_PINS_SIZE - 1);

			//reconfigure pins
			for(int idx = 0; idx < (lastFlag ? 1 : readers.size()); idx++){
				readers[idx]->changeRx(pins::RX_PINS[i + idx]);
			}

			int remainWaitMs = 0;
			const auto startTick = xTaskGetTickCount();
			for(int idx = 0; idx < (lastFlag ? 1 : readers.size()); idx++){
				//size must be 2 times bigger than payload (14 bytes)
				uint8_t buff[28] = {0};
				//160 ms is 100% guarantee to read a packet if there is no packet then there is nothing in range of rfid
				int count = readers[idx]->readAtMax({buff, sizeof(buff)}, idx == 0 ? periodMs : remainWaitMs);
				if(count <= 0){
					if(_states[i + idx] != 0){
						FB_DEBUG_LOG_I_TAG("LOST tag on: %d", i + idx);
						//we have change must report it through something
						changed = true;
						_states[i + idx] = 0;
					}
					continue;
				}

				remainWaitMs = periodMs - pdTICKS_TO_MS((xTaskGetTickCount() - startTick));
				if(remainWaitMs < 0){
					remainWaitMs = 0;
				}

				FB_DEBUG_LOG_I_TAG("READ on %d: %d", i + idx, count);

				//try to find complete package it starts with 0x02 and ends with 0x03 the size is 14 bytes total
				const int startIndex = _findPackage({buff});
				if(startIndex < 0){
					continue;
				}


				//parse tag
				// const uint64_t tagId = *(reinterpret_cast<uint64_t*>(&buff[startIndex + 3]));
				const uint64_t version = _hexstrToVal(reinterpret_cast<const char*>(&buff[startIndex + 1]), 2);
				const uint64_t tagId = _hexstrToVal(reinterpret_cast<const char*>(&buff[startIndex + 3]), 8);
				const uint64_t crc = _hexstrToVal(reinterpret_cast<const char*>(&buff[startIndex + 11]), 2);
				FB_DEBUG_LOG_W_TAG("Found tag on %d: %llu, V(%llu), CRC(%llu)", i + idx, tagId, version, crc);
				if(_states[i + idx] != tagId){
					//we have change must report it through something
					changed = true;
					_states[i + idx] = tagId;
				}
			}
		}

		if(changed){
			xTaskNotifyGive(_senderHndl);
		}

		vTaskDelay(pdMS_TO_TICKS(10));
	}

	vTaskDelete(NULL);
}

static void _init_from_settings()
{

}



void project::initHwObjs()
{
	_init_from_settings();

	_sensorService.addSensor(&_keyboardSensor);

	//register key handler for dropping WIFI settings
	global::getEventManager()->attachListener(&_keyboardHandler);

	_http.init();
	if(settings::getLog() == 0){
		esp_log_set_level_master(ESP_LOG_NONE);

		_uart_0 = new interfaces::Uart(UART_NUM_0, pins::RX_PINS[0], pins::TX_PIN, 9600);
	}

	assert(xTaskCreate(&_sendTask, "SENDER", 4 * 1024, NULL, 13, &_senderHndl) == pdPASS);
	assert(xTaskCreate(&_taskReader, "UART_READER", 4 * 1024, NULL, 15, NULL) == pdPASS);
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