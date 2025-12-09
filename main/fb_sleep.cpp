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

// int64_t _desired_sleep_us = 0;
// int64_t _actual_sleep_us = 0;
// int64_t _sleep_tries = 0;
// int _cause_timer = 0;
// int _cause_wifi = 0;
// int _cause_gpio = 0;
// int _cause_uart = 0;
// int _cause_ext = 0;
// int _cause_other = 0;



// static esp_err_t _enter(int64_t sleep_time_us, void *arg)
// {
// 	_desired_sleep_us += sleep_time_us;
// 	_sleep_tries++;

// 	return ESP_OK;
// }

// static esp_err_t _exit(int64_t sleep_time_us, void *arg)
// {
// 	_actual_sleep_us += sleep_time_us;

// 	esp_sleep_wakeup_cause_t cause = esp_sleep_get_wakeup_cause();
// 	switch(cause) {
//     case ESP_SLEEP_WAKEUP_TIMER:
// 		_cause_timer++;
//         break;
//     case ESP_SLEEP_WAKEUP_WIFI: 
// 		_cause_wifi++;
//         break;
//     case ESP_SLEEP_WAKEUP_GPIO: 
// 		_cause_gpio++;
//         break;
//     case ESP_SLEEP_WAKEUP_UART: 
// 		_cause_uart++;
//         break;
//     case ESP_SLEEP_WAKEUP_EXT0: 
// 		_cause_ext++;
//         break;
//     default: 
// 		_cause_other++;
//         break;
// 	}

// 	return ESP_OK;
// }

void fb::sleep::enable()
{
	ESP_ERROR_CHECK(esp_wifi_set_ps(WIFI_PS_MAX_MODEM));
	esp_pm_config_t pm_config = {
		.max_freq_mhz = 160,
		.min_freq_mhz = CONFIG_XTAL_FREQ,
		.light_sleep_enable = true
	};

	esp_sleep_enable_wifi_beacon_wakeup();
	esp_sleep_enable_wifi_wakeup();
	ESP_ERROR_CHECK(esp_pm_configure(&pm_config));

	// esp_pm_sleep_cbs_register_config_t cbs{};
	// cbs.enter_cb = &_enter;
	// cbs.exit_cb = &_exit;
	// esp_pm_light_sleep_register_cbs(&cbs);
}

void fb::sleep::disable()
{
	esp_pm_config_t pm_config = {
		.max_freq_mhz = 160,
		.min_freq_mhz = CONFIG_XTAL_FREQ,
		.light_sleep_enable = false
	};
	ESP_ERROR_CHECK(esp_pm_configure(&pm_config));
}
