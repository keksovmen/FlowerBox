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
#include "esp_adc/adc_oneshot.h"


//must be calibrated from chip to chip
#define _MAX_RAW (3161 - 10)
#define _MIN_RAW (2304 + 10)



using namespace fb;
using namespace project;



static void _handleEvent(const event::Event& event);
 


static BaseCore _core{&_handleEvent, "TressureBox"};
static periph::MqttClient _mqtt;
static esp_pm_lock_handle_t _lock;
static adc_oneshot_unit_handle_t _adc1_handle;



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



    adc_oneshot_unit_init_cfg_t init_config = {
        .unit_id = ADC_UNIT_1,
        .ulp_mode = ADC_ULP_MODE_DISABLE,
    };
    adc_oneshot_new_unit(&init_config, &_adc1_handle);

    adc_oneshot_chan_cfg_t config = {
        .atten = ADC_ATTEN_DB_12,              // 11dB attenuation for wider voltage range
        .bitwidth = ADC_BITWIDTH_DEFAULT,      // 12 bits by default
    };
    adc_oneshot_config_channel(_adc1_handle, ADC_CHANNEL_1, &config);


	
	_mqtt.init("mqtt://192.168.0.111:8081");
	_mqtt.addDataHandler(&_dataHandler);

	global::getEventManager()->attachListener(&_mqtt);
	global::getTimeScheduler()->addActionDelayed([](){
		//publish power level
		int adc_raw[100];
		int sum = 0;
		for (int i = 0; i < 100; i++) {
			adc_oneshot_read(_adc1_handle, ADC_CHANNEL_1, &adc_raw[i]);
			sum += adc_raw[i];
		}
		int avg = sum / 100;
		int percents = ((avg - _MIN_RAW) * 100) / (_MAX_RAW - _MIN_RAW);
		percents = percents < 0 ? 0 : percents;
		percents = percents > 100 ? 100 : percents;

		const std::string data = "{0:" + std::to_string(avg) + ", " + std::to_string(percents) + "%}";
		_mqtt.publish("/status", data);
	}, 700, 1000, true);
}

const CoreInfo& project::getInfo()
{
	return _info;
}