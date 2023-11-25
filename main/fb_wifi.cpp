#include "fb_wifi.hpp"

#include <string.h>
#include <mutex>

#include "fb_globals.hpp"

#include "esp_log.h"
#include "esp_netif.h"
#include "esp_wifi.h"



#define CONFIG_EXAMPLE_WIFI_CONN_MAX_RETRY 3



using namespace fb;
using namespace fb::wifi;



static const char* TAG = "fb_wifi";



static esp_netif_t *_sta_netif = NULL;

static int _reconnectAttempts = 0;



static void _on_wifi_disconnect(void *arg, esp_event_base_t event_base,
							   int32_t event_id, void *event_data)
{
	_reconnectAttempts++;
	if (_reconnectAttempts > CONFIG_EXAMPLE_WIFI_CONN_MAX_RETRY) {
		ESP_LOGI(TAG, "WiFi Connect failed %d times, stop reconnect.", _reconnectAttempts);
		global::getEventManager()->pushEvent({event::EventGroup::WIFI, static_cast<int>(WifiEventId::FAILED_TO_CONNECT), NULL});
		return;
	}
	ESP_LOGI(TAG, "Wi-Fi disconnected, trying to reconnect...");
	ESP_ERROR_CHECK(esp_wifi_connect());
}

static void _on_wifi_connect(void *esp_netif, esp_event_base_t event_base,
							int32_t event_id, void *event_data)
{
	ESP_LOGI(TAG, "Wifi connected");
}

static void _on_sta_got_ip(void *arg, esp_event_base_t event_base,
					  int32_t event_id, void *event_data)
{
	_reconnectAttempts = 0;
	ip_event_got_ip_t *event = (ip_event_got_ip_t *)event_data;

	ESP_LOGI(TAG, "Got IPv4 event: Interface \"%s\" address: " IPSTR, esp_netif_get_desc(event->esp_netif), IP2STR(&event->ip_info.ip));
	ESP_LOGI(TAG, "- IPv4 address: " IPSTR ",", IP2STR(&event->ip_info.ip));

	global::getEventManager()->pushEvent({event::EventGroup::WIFI, static_cast<int>(WifiEventId::CONNECTED), NULL});
}



static esp_err_t _sta_connect(wifi_config_t* wifi_config)
{
	ESP_LOGI(TAG, "%s", __FUNCTION__);

	_reconnectAttempts = 0;
	ESP_ERROR_CHECK(esp_event_handler_register(WIFI_EVENT, WIFI_EVENT_STA_CONNECTED, &_on_wifi_connect, NULL));
	ESP_ERROR_CHECK(esp_event_handler_register(WIFI_EVENT, WIFI_EVENT_STA_DISCONNECTED, &_on_wifi_disconnect, NULL));
	ESP_ERROR_CHECK(esp_event_handler_register(IP_EVENT, IP_EVENT_STA_GOT_IP, &_on_sta_got_ip, NULL));

	ESP_LOGI(TAG, "Connecting to %s...", wifi_config->sta.ssid);
	ESP_ERROR_CHECK(esp_wifi_set_config(WIFI_IF_STA, wifi_config));
	esp_err_t ret = esp_wifi_connect();
	if (ret != ESP_OK) {
		ESP_LOGE(TAG, "WiFi connect failed! ret:%x", ret);
		return ret;
	}

	return ESP_OK;
}

// static esp_err_t _sta_disconnect()
// {
// 	ESP_LOGI(TAG, "%s", __FUNCTION__);

// 	ESP_ERROR_CHECK(esp_event_handler_unregister(WIFI_EVENT, WIFI_EVENT_STA_DISCONNECTED, &_on_wifi_disconnect));
// 	ESP_ERROR_CHECK(esp_event_handler_unregister(IP_EVENT, IP_EVENT_STA_GOT_IP, &_on_sta_got_ip));
// 	ESP_ERROR_CHECK(esp_event_handler_unregister(WIFI_EVENT, WIFI_EVENT_STA_CONNECTED, &_on_wifi_connect));
// 	return esp_wifi_disconnect();
// }

// static bool _is_wifi_init()
// {
// 	wifi_mode_t mode;
// 	return ESP_ERR_WIFI_NOT_INIT != esp_wifi_get_mode(&mode);
// }


static void _create_station()
{
	ESP_LOGI(TAG, "%s", __FUNCTION__);

	if(_sta_netif){
		ESP_LOGI(TAG, "Wifi STA already created");
		return;
	}

	// Warning: the interface desc is used in tests to capture actual connection details (IP, gw, mask)
	_sta_netif = esp_netif_create_default_wifi_sta();

	ESP_ERROR_CHECK(esp_wifi_start());
}

// static void _stop()
// {
// 	ESP_LOGI(TAG, "%s", __FUNCTION__);

// 	esp_err_t err = esp_wifi_stop();
// 	if (err == ESP_ERR_WIFI_NOT_INIT) {
// 		return;
// 	}
// 	ESP_ERROR_CHECK(err);
// 	// ESP_ERROR_CHECK(esp_wifi_deinit());
// 	ESP_ERROR_CHECK(esp_wifi_clear_default_wifi_driver_and_handlers(_sta_netif));
// 	esp_netif_destroy(_sta_netif);
// 	_sta_netif = NULL;
// }



static void _init()
{
	ESP_ERROR_CHECK(esp_netif_init());
	ESP_ERROR_CHECK(esp_event_loop_create_default());

	wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
	ESP_ERROR_CHECK(esp_wifi_init(&cfg));
	ESP_ERROR_CHECK(esp_wifi_set_storage(WIFI_STORAGE_RAM));
	ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_STA));
}



bool wifi::operator==(int val, WifiEventId id)
{
	return val == static_cast<int>(id);
}



void wifi::init()
{
	ESP_LOGI(TAG, "%s", __FUNCTION__);

	static std::once_flag flag;
	std::call_once(flag, &_init);
}

esp_err_t wifi::connect(const std::string& ssid, const std::string& password)
{
	ESP_LOGI(TAG, "%s", __FUNCTION__);

	_create_station();

	wifi_config_t wifi_config;
	memset(&wifi_config, 0, sizeof(wifi_config));

	memcpy(wifi_config.sta.ssid, ssid.c_str(), ssid.length() + 1);
	memcpy(wifi_config.sta.password, password.c_str(), password.length() + 1);

	return _sta_connect(&wifi_config);
}

esp_err_t wifi::shutdown()
{
	// ESP_LOGI(TAG, "%s", __FUNCTION__);

	// esp_err_t err = _sta_disconnect();
	// _stop();

	// return err;
	return ESP_FAIL;
}