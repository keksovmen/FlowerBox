#include "fb_core.hpp"

#include "fb_globals.hpp"
#include "fb_tressure_box_obj.hpp"
#include "fb_tressure_hw_obj.hpp"
#include "fb_tressure_mapper_obj.hpp"
#include "fb_tressure_pins.hpp"
#include "fb_wifi.hpp"

#include "esp_wifi.h"
#include "esp_pm.h"
#include "esp_sleep.h"



using namespace fb;
using namespace project;



static void _handleEvent(const event::Event& event);
 


static BaseCore _core{&_handleEvent, "TressureBox"};



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







void project::bootInit()
{
	
}

const CoreInfo& project::getInfo()
{
	return _info;
}