#include "fb_tressure_hw_obj.hpp"

#include "fb_clock.hpp"
#include "fb_globals.hpp"
#include "fb_keyboard_handler.hpp"
#include "fb_sensor_keyboard.hpp"
#include "fb_tressure_pins.hpp"
#include "fb_tressure_settings.hpp"

#include <cJSON.h>

#include "esp_pm.h"
#include "esp_wifi.h"



#define _MQTT_PATH_STATUS "/status"
#define _MQTT_PATH_DEBUG "/debug"
#define _MQTT_PATH_LOCK "/controls/lock"
#define _MQTT_PATH_RGB "/controls/rgb"



using namespace fb;
using namespace project;



static const char* TAG = "HW";



static sensor::KeyboardSensorIrq<2> _keyboardSensor({
	std::pair{pins::PIN_KEYBOARD, h::ButtonVK::VK_0},
	std::pair{pins::PIN_LOCK_SENSOR, h::ButtonVK::VK_1}});

static adc::AdcPin _adc;
static adc::Battery _battery(_adc);
static wrappers::WrapperGpio _lockPin(static_cast<gpio_num_t>(pins::PIN_LOCK), false);
static wrappers::WrapperPwm _ledRPin(LEDC_TIMER_0, LEDC_CHANNEL_0, static_cast<gpio_num_t>(pins::PIN_LED_R), false);
static wrappers::WrapperPwm _ledGPin(LEDC_TIMER_0, LEDC_CHANNEL_1, static_cast<gpio_num_t>(pins::PIN_LED_G), false);
static wrappers::WrapperPwm _ledBPin(LEDC_TIMER_0, LEDC_CHANNEL_2, static_cast<gpio_num_t>(pins::PIN_LED_B), false);
static periph::MqttClient _mqtt;

static sensor::SensorService _sensorService;
static switches::SwitchService _swithService;

static sensor::SensorStorage _sensorStorage;

static keyboard::KeyboardHandler _keyboardHandler;
static esp_pm_lock_handle_t _lock;
static bool _doorClosed = true;



static int _getIntFromJsonOrDefault(cJSON* json, std::string_view key, int def)
{
	auto* obj = cJSON_GetObjectItem(json, key.cbegin());
	if(obj != nullptr){
		return static_cast<int>(cJSON_GetNumberValue(obj));
	}

	return def;
}



static void _pulse()
{
	esp_pm_lock_acquire(_lock);

	_lockPin.setValue(1);
	//TODO: add duration to settings
	//TODO: if for current power level need to start using longer delay after 3.5V
	vTaskDelay(pdMS_TO_TICKS(150));
	_lockPin.setValue(0);

	esp_pm_lock_release(_lock);
}

static void _handleLockTopic(std::string_view data)
{
	cJSON* obj = cJSON_ParseWithLength(data.begin(), data.length());
	const int id = _getIntFromJsonOrDefault(obj, "ID", -1);

	cJSON_Delete(obj);

	if(id != settings::getMqttId()){
		FB_DEBUG_LOG_I_TAG("Not my MQTT id: %d != %d", id, settings::getMqttId());
		return;
	}

	//generate pulse
	_pulse();
}

static void _handleRGBTopic(std::string_view data)
{
	cJSON* obj = cJSON_ParseWithLength(data.begin(), data.length());
	const int id = _getIntFromJsonOrDefault(obj, "ID", -1);
	const int r = _getIntFromJsonOrDefault(obj, "R", 0);
	const int g = _getIntFromJsonOrDefault(obj, "G", 0);
	const int b = _getIntFromJsonOrDefault(obj, "B", 0);

	cJSON_Delete(obj);

	if(id != settings::getMqttId()){
		FB_DEBUG_LOG_I_TAG("Not my MQTT id: %d != %d", id, settings::getMqttId());
		return;
	}

	settings::setLedRed(r);
	settings::setLedGreen(g);
	settings::setLedBlue(b);

	if(!_doorClosed){
		_ledRPin.setValue(r);
		_ledGPin.setValue(g);
		_ledBPin.setValue(b);
	}
}

static void _dataHandler(std::string_view topic, std::string_view data)
{
	//parse json data and see if you must fire
	FB_DEBUG_LOG_I_TAG("Data handler: %.*s -> %.*s", topic.length(), topic.cbegin(), data.length(), data.cbegin());

	if(topic == _MQTT_PATH_LOCK){
		_handleLockTopic(data);
	}else if(topic == _MQTT_PATH_RGB){
		_handleRGBTopic(data);
	}else{
		FB_DEBUG_LOG_W_TAG("Unexpected MQTT topic!");
	}
}



static void _batteryAction()
{
	wifi_ap_record_t ap_info;
	const int err = esp_wifi_sta_get_ap_info(&ap_info);
	FB_DEBUG_LOG_I_TAG("RSSI: %d", ap_info.rssi);

	int avg = _adc.readRaw(1000);
	int percents = _battery.readCharge();
	float volts = _battery.readVolts();

	char buff[256];
	sprintf(buff, "{\"ID\":%d, \"RSSI\":%d, \"time\":%lld, \"battery\":{\"raw\":%d, \"percents\":%d, \"volts\":%.3f}}",
		settings::getMqttId(), ap_info.rssi, static_cast<clock::Timestamp>(clock::getCurrentTime()), avg, percents, volts);

	FB_DEBUG_LOG_I_TAG("Battery: %d raw, %d %%, volts %.3f", avg, percents, volts);

	// extern int64_t _desired_sleep_us;
	// extern int64_t _actual_sleep_us;
	// extern int64_t _sleep_tries;
	// extern int _cause_timer;
	// extern int _cause_wifi;
	// extern int _cause_gpio;
	// extern int _cause_uart;
	// extern int _cause_ext;
	// extern int _cause_other;

	if(err == ESP_OK){
		_mqtt.publish(_MQTT_PATH_STATUS, buff);
	}

	// sprintf(buff, "CAUSES: timer %d, wifi %d, gpio %d, uart %d, ext %d, other %d", _cause_timer, _cause_wifi, _cause_gpio, _cause_uart, _cause_ext, _cause_other);
	// FB_DEBUG_LOG_E_TAG("Sleep cycles: %lld, desired: %lld, actual: %lld, delta %lld", _sleep_tries, _desired_sleep_us, _actual_sleep_us, static_cast<clock::Timestamp>(clock::getCurrentTime()) * 1000000 - _actual_sleep_us);
	// FB_DEBUG_LOG_E_TAG("%s", buff);

	// if(err == ESP_OK){
	// 	_mqtt.publish(_MQTT_PATH_DEBUG, buff);
	// }
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
	_ledRPin.init();
	_ledGPin.init();
	_ledBPin.init();

	// gpio_sleep_set_direction(static_cast<gpio_num_t>(pins::PIN_LOCK), GPIO_MODE_OUTPUT);
	// gpio_sleep_set_pull_mode(static_cast<gpio_num_t>(pins::PIN_LOCK), GPIO_FLOATING);
	// gpio_sleep_sel_dis(static_cast<gpio_num_t>(pins::PIN_LOCK));


	//read from settings
	_mqtt.init("mqtt://" + settings::getIp() + ":" + std::to_string(settings::getPort()));
	_mqtt.addDataHandler(&_dataHandler);
	_mqtt.registerSubscribeHandler([](const auto& handler){
		std::invoke(handler, _MQTT_PATH_LOCK, 2);
		std::invoke(handler, _MQTT_PATH_RGB, 2);
	});

	esp_pm_lock_create(ESP_PM_NO_LIGHT_SLEEP, 0, "pin_action", &_lock);


	_init_from_settings();


	global::getEventManager()->attachListener(&_keyboardHandler);
	global::getEventManager()->attachListener(&_mqtt);
	global::getTimeScheduler()->addActionDelayed(&_batteryAction, 60000, 1000, true);
	// global::getTimeScheduler()->addActionDelayed([](){
	// 	wifi_ap_record_t ap_info;
	// 	esp_err_t err = esp_wifi_sta_get_ap_info(&ap_info);
	// 	FB_DEBUG_LOG_W_TAG("RSSI: %d", ap_info.rssi);

	// }, 500, 1000, true);

	_sensorService.addSensor(&_keyboardSensor);
	_keyboardSensor.init();
	_keyboardSensor.update();
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

void project::doorIsOpened()
{
	if(!_doorClosed){
		return;
	}

	FB_DEBUG_LOG_I_TAG("Door is opened!");

	_doorClosed = false;
	//enable GPIO if not enabled yet
	//read from NVS
	_ledRPin.setValue(settings::getLedRed());
	_ledGPin.setValue(settings::getLedGreen());
	_ledBPin.setValue(settings::getLedBlue());
}

void project::doorIsClosed()
{
	if(_doorClosed){
		return;
	}

	FB_DEBUG_LOG_I_TAG("Door is closed!");

	_doorClosed = true;
	//disable GPIO
	_ledRPin.setValue(0);
	_ledGPin.setValue(0);
	_ledBPin.setValue(0);
}

void project::forceStatusPost()
{
	_batteryAction();
}