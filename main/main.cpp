/* LwIP SNTP example

   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/
#include <string.h>
#include <time.h>
#include <sys/time.h>

#include "driver/gpio.h"
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

#include "fb_state_manager.hpp"
#include "fb_state_booted.hpp"



static const char *TAG = "main";

RTC_DATA_ATTR static int _boot_count = 0;
static gpio_num_t _led_pins[] = {GPIO_NUM_5, GPIO_NUM_3, GPIO_NUM_4, GPIO_NUM_19, GPIO_NUM_18};



static void _init_gpio()
{
	ESP_LOGI(TAG, "_init_gpio()");

	for(int i = 0; i < sizeof(_led_pins) / sizeof(_led_pins[0]); i++){
		ESP_LOGI(TAG, "Setting index = %d, gpio = %d", i, _led_pins[i]);

		assert(GPIO_IS_VALID_OUTPUT_GPIO(_led_pins[i]));

		gpio_config_t cfg = {
			.pin_bit_mask = 1ULL << _led_pins[i],
			.mode = GPIO_MODE_OUTPUT,
			.pull_up_en = GPIO_PULLUP_DISABLE,
			.pull_down_en = GPIO_PULLDOWN_DISABLE,
			.intr_type = GPIO_INTR_DISABLE,
		};
		gpio_config(&cfg);

		gpio_set_level(_led_pins[i], 0);
	}
}



extern "C" void app_main(void)
{
	ESP_LOGI(TAG, "Booting times = %d", _boot_count);
	
	_init_gpio();
	gpio_set_level(_led_pins[_boot_count % (sizeof(_led_pins) / sizeof(_led_pins[0]))], 1);
	ESP_LOGI(TAG, "Level of %d = %d", _boot_count % (sizeof(_led_pins) / sizeof(_led_pins[0])), gpio_get_level(_led_pins[_boot_count % (sizeof(_led_pins) / sizeof(_led_pins[0]))]));

	auto manager = fb::state::StateManager("MainStateManager");
	manager.init(std::unique_ptr<fb::state::State>(new fb::state::StateBooted(manager)));

	time_t now;
	struct tm timeinfo;
	time(&now);
	// localtime_r(&now, &timeinfo);
	// // Is time set? If not, tm_year will be (1970 - 1900).
	// if (timeinfo.tm_year < (2023 - 1900)) {
	// 	ESP_LOGI(TAG, "Time is not set yet. Connecting to WiFi and getting time over NTP.");
	// 	_obtain_time();
	// 	// update 'now' variable with current time
	// 	time(&now);
	// }

	char strftime_buf[64];

	// Set timezone to Eastern Standard Time and print local time
	setenv("TZ", "GMT-3", 1);
	tzset();
	localtime_r(&now, &timeinfo);
	strftime(strftime_buf, sizeof(strftime_buf), "%Y:%m:%d\t%H:%M:%S", &timeinfo);
	ESP_LOGI(TAG, "The current date/time in Moscow is: %s", strftime_buf);

	
	_boot_count++;

	// vTaskDelay(pdMS_TO_TICKS(1000));
	gpio_set_level(_led_pins[_boot_count % (sizeof(_led_pins) / sizeof(_led_pins[0]))], 0);

	const int deep_sleep_sec = 1;
	ESP_LOGI(TAG, "Entering deep sleep for %d seconds", deep_sleep_sec);
	esp_deep_sleep(1000000LL * deep_sleep_sec);
}