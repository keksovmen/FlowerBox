#include "fb_clock.hpp"

#include <string.h>
#include <time.h>
#include <sys/time.h>

#include "esp_log.h"
#include "esp_netif_sntp.h"
#include "esp_sntp.h"
#include "lwip/ip_addr.h"


using namespace fb;
using namespace clock;



static const char* TAG = "fb_clock";



static void _print_servers(void)
{
	ESP_LOGI(TAG, "List of configured NTP servers:");

	for (uint8_t i = 0; i < SNTP_MAX_SERVERS; ++i){
		if (esp_sntp_getservername(i)){
			ESP_LOGI(TAG, "server %d: %s", i, esp_sntp_getservername(i));
		} else {
			// we have either IPv4 or IPv6 address, let's print it
			char buff[128];
			ip_addr_t const *ip = esp_sntp_getserver(i);
			if (ipaddr_ntoa_r(ip, buff, sizeof(buff)) != NULL)
				ESP_LOGI(TAG, "server %d: %s", i, buff);
		}
	}
}

bool clock::syncTime()
{
	/*
	 * This is the basic default config with one server and starting the service
	 */
	esp_sntp_config_t config = ESP_NETIF_SNTP_DEFAULT_CONFIG(CONFIG_SNTP_TIME_SERVER);
	esp_netif_sntp_init(&config);

	_print_servers();

	// wait for time to be set
	time_t now = 0;
	struct tm timeinfo;
	int retry = 0;
	const int retry_count = 15;
	while (esp_netif_sntp_sync_wait(2000 / portTICK_PERIOD_MS) == ESP_ERR_TIMEOUT && ++retry < retry_count) {
		ESP_LOGI(TAG, "Waiting for system time to be set... (%d/%d)", retry, retry_count);
	}

	time(&now);
	localtime_r(&now, &timeinfo);
	esp_netif_sntp_deinit();

	return true;
}