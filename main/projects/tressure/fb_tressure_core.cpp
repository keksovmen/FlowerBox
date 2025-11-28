#include "fb_core.hpp"

#include "fb_globals.hpp"
#include "fb_mqtt_client.hpp"
#include "fb_tressure_box_obj.hpp"
#include "fb_tressure_hw_obj.hpp"
#include "fb_tressure_mapper_obj.hpp"
#include "fb_tressure_pins.hpp"
#include "fb_wifi.hpp"

#include "esp_wifi.h"
#include "esp_pm.h"
#include "esp_sleep.h"
#include "driver/gpio.h"



using namespace fb;
using namespace project;



static void _handleEvent(const event::Event& event);
 


static BaseCore _core{&_handleEvent, "TressureBox"};
static periph::MqttClient _mqtt;
static esp_pm_lock_handle_t _lock;



static CoreInfo _info = {
	.core = _core,
	.requiresTime = true,
};



static void _handleEvent(const event::Event& event)
{
	//do stuff
	if(event.groupId == event::EventGroup::WIFI && event.eventId == wifi::WifiEventId::CONNECTED){
		// ESP_ERROR_CHECK(esp_wifi_set_protocol(WIFI_IF_STA, WIFI_PROTOCOL_11N));
		ESP_ERROR_CHECK(esp_wifi_set_ps(WIFI_PS_MAX_MODEM));
		esp_pm_config_t pm_config = {
			.max_freq_mhz = 160,
			.min_freq_mhz = 80,
			.light_sleep_enable = true
		};
		esp_sleep_enable_timer_wakeup(1000000);
		esp_sleep_enable_wifi_beacon_wakeup();
		esp_sleep_enable_wifi_wakeup();
		ESP_ERROR_CHECK(esp_pm_configure(&pm_config));
	}
}



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



void project::bootInit()
{
	esp_pm_lock_create(ESP_PM_NO_LIGHT_SLEEP, 0,
        "debug", &_lock);
	gpio_config_t io_conf = {};
		io_conf.intr_type = GPIO_INTR_DISABLE;         // Disable interrupt
		io_conf.mode = GPIO_MODE_OUTPUT;               // Set as output mode
		io_conf.pin_bit_mask = (1ULL << 10);     // Bit mask for GPIO8
		io_conf.pull_down_en = GPIO_PULLDOWN_DISABLE;                      // Disable pull-down
		io_conf.pull_up_en = GPIO_PULLUP_DISABLE;                        // Disable pull-up
	gpio_config(&io_conf);
	gpio_set_level(static_cast<gpio_num_t>(pins::PIN_LOCK), 0);
	
	_mqtt.init("mqtt://192.168.0.111:8081");
	_mqtt.addDataHandler(&_dataHandler);

	global::getEventManager()->attachListener(&_mqtt);
	global::getTimeScheduler()->addActionDelayed([](){
		//publish power level
		_mqtt.publish("/status", "{0:1}");
	}, 6000, 1000, true);
}

const CoreInfo& project::getInfo()
{
	return _info;
}