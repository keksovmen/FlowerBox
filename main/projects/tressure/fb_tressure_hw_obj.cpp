#include "fb_tressure_hw_obj.hpp"

#include "fb_globals.hpp"
#include "fb_keyboard_handler.hpp"
#include "fb_sensor_keyboard.hpp"
#include "fb_tressure_pins.hpp"
#include "fb_tressure_settings.hpp"

#include <cJSON.h>

#include "esp_pm.h"
// #include "esp_wifi.h"



using namespace fb;
using namespace project;



static const char* TAG = "HW";



static sensor::KeyboardSensor<1> _keyboardSensor({std::pair{pins::PIN_KEYBOARD, h::ButtonVK::VK_0}});

static adc::AdcPin _adc;
static adc::Battery _battery(_adc);
static wrappers::WrapperGpio _lockPin(static_cast<gpio_num_t>(pins::PIN_LOCK), false);
static periph::MqttClient _mqtt;

static sensor::SensorService _sensorService;
static switches::SwitchService _swithService;

static sensor::SensorStorage _sensorStorage;

static keyboard::KeyboardHandler _keyboardHandler;
static esp_pm_lock_handle_t _lock;



static void _pulse()
{
	esp_pm_lock_acquire(_lock);

	_lockPin.setValue(1);
	//TODO: add duration to settings
	vTaskDelay(pdMS_TO_TICKS(50));
	_lockPin.setValue(0);

	esp_pm_lock_release(_lock);
}

static void _dataHandler(std::string_view topic, std::string_view data)
{
	//parse json data and see if you must fire
	FB_DEBUG_LOG_I_TAG("Data handler: %.*s -> %.*s", topic.length(), topic.cbegin(), data.length(), data.cbegin());
	int id = -1;

	cJSON* obj = cJSON_ParseWithLength(data.begin(), data.length());
	cJSON* item = cJSON_GetObjectItem(obj, "ID");
	if(item != nullptr){
		id = cJSON_GetNumberValue(item);
	}

	FB_DEBUG_LOG_W_TAG("OBJ -> %p, item -> %p", obj, item);

	cJSON_Delete(obj);

	if(id != settings::getMqttId()){
		FB_DEBUG_LOG_I_TAG("Not my MQTT id: %d != %d", id, settings::getMqttId());
		return;
	}

	//generate pulse
	_pulse();
}

static void _batteryAction()
{
	int avg = _adc.readRaw(100);
	int percents = _battery.readCharge();
	const std::string data = "{" + std::to_string(settings::getMqttId()) + ":" + std::to_string(avg) + ", " + std::to_string(percents) + "%}";
	_mqtt.publish("/status", data);

	FB_DEBUG_LOG_I_TAG("Battery: %d raw, %d %%", avg, percents);
}

static void _init_from_settings()
{
	_battery.setMinRaw(settings::getBatteryMin());
	_battery.setMaxRaw(settings::getBatteryMax());
}



void project::initHwObjs()
{
	_adc.init(ADC_UNIT_1, static_cast<adc_channel_t>(pins::PIN_BATTERY), ADC_ATTEN_DB_12);
	_lockPin.init();

	//read from settings
	_mqtt.init("mqtt://" + settings::getIp() + ":" + std::to_string(settings::getPort()));
	_mqtt.addDataHandler(&_dataHandler);
	_mqtt.registerSubscribeHandler([](const auto& handler){
		std::invoke(handler, "/controls", 2);
	});

	esp_pm_lock_create(ESP_PM_NO_LIGHT_SLEEP, 0, "pin_action", &_lock);


	_init_from_settings();


	global::getEventManager()->attachListener(&_keyboardHandler);
	global::getEventManager()->attachListener(&_mqtt);
	global::getTimeScheduler()->addActionDelayed(&_batteryAction, 30000, 1000, true);
	// global::getTimeScheduler()->addActionDelayed([](){
	// 	wifi_ap_record_t ap_info;
	// 	esp_err_t err = esp_wifi_sta_get_ap_info(&ap_info);
	// 	FB_DEBUG_LOG_W_TAG("RSSI: %d", ap_info.rssi);

	// }, 500, 1000, true);
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

adc::AdcPin& project::getAdcPinHw()
{
	return _adc;
}

adc::Battery& project::getBatteryHw()
{
	return _battery;
}

wrappers::WrapperGpio& project::getPulseGpio()
{
	return _lockPin;	
}
