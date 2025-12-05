#include "fb_sleep.hpp"

#include "esp_pm.h"
#include "esp_sleep.h"
#include "esp_wifi.h"



using namespace fb;
using namespace sleep;



// bool fb::sleep::operator==(int val, SleepEvent id)
// {
// 	return val == static_cast<int>(id);
// }

void fb::sleep::enable()
{
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

void fb::sleep::disable()
{
	esp_pm_config_t pm_config = {
		.max_freq_mhz = 160,
		.min_freq_mhz = 80,
		.light_sleep_enable = false
	};
	ESP_ERROR_CHECK(esp_pm_configure(&pm_config));
}
