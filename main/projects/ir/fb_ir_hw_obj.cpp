#include "fb_ir_hw_obj.hpp"

#include "fb_globals.hpp"
#include "fb_keyboard_handler.hpp"
#include "fb_ir_pins.hpp"
#include "fb_ir_settings.hpp"
#include "fb_mqtt_client.hpp"
#include "fb_sensor_keyboard.hpp"
#include "fb_ir_reader.hpp"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "driver/gpio.h"
#include "soc/gpio_struct.h"
#include "esp_attr.h"
#include "esp_timer.h"



using namespace fb;
using namespace project;



static const char* TAG = "HW";



static sensor::SensorService _sensorService;
static switches::SwitchService _swithService;

static sensor::SensorStorage _sensorStorage;

static keyboard::KeyboardHandler _keyboardHandler;
static sensor::KeyboardSensor<1> _keyboardSensor({std::pair{pins::PIN_KEYBOARD_RESET, h::ButtonVK::VK_0}});
static DRAM_ATTR periph::IrReader _irReader;

// static periph::MqttClient _mqtt;



static void _init_from_settings()
{

}

// static void _mqttData_handler(std::string_view topic, std::string_view data)
// {
// 	//start actions
// 	if(data.starts_with("start")){
// 		// memset(s_cycle_buffer, 1, sizeof(s_cycle_buffer));
// 		// s_buffer_idx = 0;
// 		oscillations = 0;
// 		triggered = 0;
// 		s_stop_polling = false;
// 		// xTaskCreate(&polling_task_function, "Pooling", 3096, NULL, 24, NULL);
// 		xTaskCreate(&_analysis_task, "Pooling", 3096, NULL, 24, NULL);
// 		gpio_intr_enable(MONITOR_GPIO);
// 	}else{
// 		//delete actions
// 		s_stop_polling = true;
// 		gpio_intr_disable(MONITOR_GPIO); 
// 	}
// }


void project::initHwObjs()
{
	_sensorService.addSensor(&_keyboardSensor);

	_init_from_settings();

	_irReader.startTask(15, 4096, [](int pin, uint32_t v){
		FB_DEBUG_LOG_I_TAG("Data CB: %d = %u", pin, v);
	});
	_irReader.init();
	_irReader.addPin(6);
	_irReader.addPin(7);
	_irReader.addPin(10);
	_irReader.addPin(2);
	_irReader.addPin(3);

	// _mqtt.init(settings::getIp(), settings::getPort());
	// _mqtt.addDataHandler(&_mqttData_handler);
	// _mqtt.registerSubscribeHandler([](auto r){
	// 	std::invoke(r, "/ir/action", 2);
	// });


	//register key handler for dropping WIFI settings
	global::getEventManager()->attachListener(&_keyboardHandler);
	// global::getEventManager()->attachListener(&_mqtt);
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