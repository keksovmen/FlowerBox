#include "fb_tressure_hw_obj.hpp"

#include "fb_globals.hpp"
#include "fb_keyboard_handler.hpp"
#include "fb_tressure_pins.hpp"
#include "fb_tressure_settings.hpp"

#include "esp_pm.h"



using namespace fb;
using namespace project;



static adc::AdcPin _adc;
static adc::Battery _battery(_adc);
static wrappers::WrapperGpio _lockPin(static_cast<gpio_num_t>(pins::PIN_LOCK), false);
static periph::MqttClient _mqtt;

static sensor::SensorService _sensorService;
static switches::SwitchService _swithService;

static sensor::SensorStorage _sensorStorage;

static keyboard::KeyboardHandler _keyboardHandler;
static esp_pm_lock_handle_t _lock;



static void _dataHandler(std::string_view topic, std::string_view data)
{
	//parse json data and see if you must fire

	//generate pulse
	esp_pm_lock_acquire(_lock);
	gpio_set_level(static_cast<gpio_num_t>(pins::PIN_LOCK), 1);
	vTaskDelay(pdMS_TO_TICKS(400));

	gpio_set_level(static_cast<gpio_num_t>(pins::PIN_LOCK), 0);
	esp_pm_lock_release(_lock);
}

static void _batteryAction()
{
	int avg = _adc.readRaw(100);
	int percents = _battery.readCharge();
	const std::string data = "{0:" + std::to_string(avg) + ", " + std::to_string(percents) + "%}";

	_mqtt.publish("/status", data);
}

static void _init_from_settings()
{
	_battery.setMinRaw(settings::getBatteryMin());
	_battery.setMaxRaw(settings::getBatteryMax());
}



void project::initHwObjs()
{
	_adc.init(ADC_UNIT_1, static_cast<adc_channel_t>(pins::PIN_BATTERY), ADC_ATTEN_DB_12);

	//read from settings
	_mqtt.init("mqtt://" + settings::getIp() + ":" + std::to_string(settings::getPort()));
	_mqtt.addDataHandler(&_dataHandler);

	esp_pm_lock_create(ESP_PM_NO_LIGHT_SLEEP, 0, "pin_action", &_lock);


	_init_from_settings();


	global::getEventManager()->attachListener(&_keyboardHandler);
	global::getEventManager()->attachListener(&_mqtt);
	global::getTimeScheduler()->addActionDelayed(&_batteryAction, 700, 1000, true);
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
