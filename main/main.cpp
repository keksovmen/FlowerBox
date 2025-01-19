/* LwIP SNTP example

   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/
#include <string.h>
#include <time.h>
#include <sys/time.h>

#include "esp_attr.h"
#include "esp_event.h"
#include "esp_log.h"
#include "esp_netif_sntp.h"
#include "esp_netif.h"
#include "esp_sleep.h"
#include "esp_sntp.h"
#include "esp_system.h"
#include "lwip/ip_addr.h"
#include "nvs_flash.h"

#include "fb_state_booted.hpp"
#include "fb_state_sensor_init.hpp"
#include "fb_globals.hpp"
#include "fb_server_debug.hpp"



static const char *TAG = "main";

// RTC_DATA_ATTR static int _boot_count = 0;



extern "C" void app_main(void)
{
	//init copy logs in to buffer for later transmiting
	fb::server::initServerDebug();
	
	// ESP_LOGI(TAG, "Booting times = %d", _boot_count);
	ESP_LOGI(TAG, "Booting");
	//инициализируем все синглетоны
	fb::global::init();

	auto* stateManager = fb::global::getStateManager();
	stateManager->init(std::make_unique<fb::state::StateBooted>(*stateManager));

	// fb::global::getEventManager()->attachListener(fb::global::getPinManager());
	fb::global::getEventManager()->attachListener(stateManager);
	fb::global::getEventManager()->pushEvent({fb::event::EventGroup::BOOT, 0, NULL});

	for(;;){
		ESP_LOGW(TAG, "Free memory %d", xPortGetFreeHeapSize());
		ESP_LOGW(TAG, "Minimum heap available %d", xPortGetMinimumEverFreeHeapSize());
		ESP_LOGW(TAG, "Stack value: %d", uxTaskGetStackHighWaterMark(NULL));
		fb::global::getEventManager()->dispatchEvent();
	}
	// fb::global::getEventManager()->pushEvent({fb::event::EventGroup::BOOT, 0, (void*)(_boot_count % 5)});
	// fb::global::getEventManager()->dispatchEvent();

	
	// manager->init(std::make_unique<fb::state::StateBooted>(manager));

	// time_t now;
	// struct tm timeinfo;
	// time(&now);
	// localtime_r(&now, &timeinfo);
	// // Is time set? If not, tm_year will be (1970 - 1900).
	// if (timeinfo.tm_year < (2023 - 1900)) {
	// 	ESP_LOGI(TAG, "Time is not set yet. Connecting to WiFi and getting time over NTP.");
	// 	_obtain_time();
	// 	// update 'now' variable with current time
	// 	time(&now);
	// }

	// char strftime_buf[64];

	// Set timezone to Eastern Standard Time and print local time
	// setenv("TZ", "GMT-3", 1);
	// tzset();
	// localtime_r(&now, &timeinfo);
	// strftime(strftime_buf, sizeof(strftime_buf), "%Y:%m:%d\t%H:%M:%S", &timeinfo);
	// ESP_LOGI(TAG, "The current date/time in Moscow is: %s", strftime_buf);

	
	// _boot_count++;

	// vTaskDelay(pdMS_TO_TICKS(1000));
	// gpio_set_level(_led_pins[_boot_count % (sizeof(_led_pins) / sizeof(_led_pins[0]))], 0);

	// const int deep_sleep_sec = 1;
	// ESP_LOGI(TAG, "Entering deep sleep for %d seconds", deep_sleep_sec);
	// esp_deep_sleep(1000000LL * deep_sleep_sec);
}