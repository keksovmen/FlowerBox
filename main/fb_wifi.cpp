#include "fb_wifi.hpp"

#include <string.h>
#include <mutex>

#include "esp_log.h"
#include "esp_netif.h"
#include "esp_wifi.h"




#define CONFIG_EXAMPLE_WIFI_CONN_MAX_RETRY 3



using namespace fb;
using namespace fb::wifi;



static const char* TAG = "fb_wifi";



static esp_netif_t *s_example_sta_netif = NULL;
static SemaphoreHandle_t s_semph_get_ip_addrs = NULL;

static int s_retry_num = 0;



static void _on_wifi_disconnect(void *arg, esp_event_base_t event_base,
							   int32_t event_id, void *event_data)
{
	s_retry_num++;
	if (s_retry_num > CONFIG_EXAMPLE_WIFI_CONN_MAX_RETRY) {
		ESP_LOGI(TAG, "WiFi Connect failed %d times, stop reconnect.", s_retry_num);
		/* let _sta_connect() return */
		if (s_semph_get_ip_addrs) {
			xSemaphoreGive(s_semph_get_ip_addrs);
		}
		return;
	}
	ESP_LOGI(TAG, "Wi-Fi disconnected, trying to reconnect...");
	esp_err_t err = esp_wifi_connect();
	if (err == ESP_ERR_WIFI_NOT_STARTED) {
		return;
	}
	ESP_ERROR_CHECK(err);
}

static void _on_wifi_connect(void *esp_netif, esp_event_base_t event_base,
							int32_t event_id, void *event_data)
{
	ESP_LOGI(TAG, "Wifi connected");
}

static void _on_sta_got_ip(void *arg, esp_event_base_t event_base,
					  int32_t event_id, void *event_data)
{
	s_retry_num = 0;
	ip_event_got_ip_t *event = (ip_event_got_ip_t *)event_data;

	ESP_LOGI(TAG, "Got IPv4 event: Interface \"%s\" address: " IPSTR, esp_netif_get_desc(event->esp_netif), IP2STR(&event->ip_info.ip));
	if (s_semph_get_ip_addrs) {
		xSemaphoreGive(s_semph_get_ip_addrs);
	} else {
		ESP_LOGI(TAG, "- IPv4 address: " IPSTR ",", IP2STR(&event->ip_info.ip));
	}
}

static esp_err_t _sta_connect(wifi_config_t* wifi_config, bool wait)
{
	ESP_LOGI(TAG, "%s", __FUNCTION__);

	if (wait) {
		s_semph_get_ip_addrs = xSemaphoreCreateBinary();
		if (s_semph_get_ip_addrs == NULL) {
			return ESP_ERR_NO_MEM;
		}
	}
	s_retry_num = 0;
	ESP_ERROR_CHECK(esp_event_handler_register(WIFI_EVENT, WIFI_EVENT_STA_DISCONNECTED, &_on_wifi_disconnect, NULL));
	ESP_ERROR_CHECK(esp_event_handler_register(IP_EVENT, IP_EVENT_STA_GOT_IP, &_on_sta_got_ip, NULL));
	ESP_ERROR_CHECK(esp_event_handler_register(WIFI_EVENT, WIFI_EVENT_STA_CONNECTED, &_on_wifi_connect, s_example_sta_netif));

	ESP_LOGI(TAG, "Connecting to %s...", wifi_config->sta.ssid);
	ESP_ERROR_CHECK(esp_wifi_set_config(WIFI_IF_STA, wifi_config));
	esp_err_t ret = esp_wifi_connect();
	if (ret != ESP_OK) {
		ESP_LOGE(TAG, "WiFi connect failed! ret:%x", ret);
		return ret;
	}

	if (wait) {
		ESP_LOGI(TAG, "Waiting for IP(s)");
		xSemaphoreTake(s_semph_get_ip_addrs, portMAX_DELAY);
		if (s_retry_num > CONFIG_EXAMPLE_WIFI_CONN_MAX_RETRY) {
			return ESP_FAIL;
		}
	}
	return ESP_OK;
}

static esp_err_t _sta_disconnect()
{
	ESP_LOGI(TAG, "%s", __FUNCTION__);

	ESP_ERROR_CHECK(esp_event_handler_unregister(WIFI_EVENT, WIFI_EVENT_STA_DISCONNECTED, &_on_wifi_disconnect));
	ESP_ERROR_CHECK(esp_event_handler_unregister(IP_EVENT, IP_EVENT_STA_GOT_IP, &_on_sta_got_ip));
	ESP_ERROR_CHECK(esp_event_handler_unregister(WIFI_EVENT, WIFI_EVENT_STA_CONNECTED, &_on_wifi_connect));
	if (s_semph_get_ip_addrs) {
		vSemaphoreDelete(s_semph_get_ip_addrs);
	}
	return esp_wifi_disconnect();
}

static void _init()
{
	ESP_ERROR_CHECK(esp_netif_init());
	ESP_ERROR_CHECK(esp_event_loop_create_default());
}

void wifi::init()
{
	ESP_LOGI(TAG, "%s", __FUNCTION__);

	static std::once_flag flag;
	std::call_once(flag, &_init);
}

void wifi::start()
{
	ESP_LOGI(TAG, "%s", __FUNCTION__);

	wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
	ESP_ERROR_CHECK(esp_wifi_init(&cfg));

	// Warning: the interface desc is used in tests to capture actual connection details (IP, gw, mask)
	s_example_sta_netif = esp_netif_create_default_wifi_sta();
	esp_wifi_set_default_wifi_sta_handlers();

	ESP_ERROR_CHECK(esp_wifi_set_storage(WIFI_STORAGE_RAM));
	ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_STA));
	ESP_ERROR_CHECK(esp_wifi_start());
}


void wifi::stop()
{
	ESP_LOGI(TAG, "%s", __FUNCTION__);

	esp_err_t err = esp_wifi_stop();
	if (err == ESP_ERR_WIFI_NOT_INIT) {
		return;
	}
	ESP_ERROR_CHECK(err);
	ESP_ERROR_CHECK(esp_wifi_deinit());
	ESP_ERROR_CHECK(esp_wifi_clear_default_wifi_driver_and_handlers(s_example_sta_netif));
	esp_netif_destroy(s_example_sta_netif);
	s_example_sta_netif = NULL;
}

esp_err_t wifi::connect(const std::string& ssid, const std::string& password)
{
	ESP_LOGI(TAG, "%s", __FUNCTION__);

	start();

	wifi_config_t wifi_config;
	memset(&wifi_config, 0, sizeof(wifi_config));

	memcpy(wifi_config.sta.ssid, ssid.c_str(), ssid.length() + 1);
	memcpy(wifi_config.sta.password, password.c_str(), password.length() + 1);

	return _sta_connect(&wifi_config, true);
}

esp_err_t wifi::shutdown()
{
	ESP_LOGI(TAG, "%s", __FUNCTION__);

	esp_err_t err = _sta_disconnect();
	stop();

	return err;
}