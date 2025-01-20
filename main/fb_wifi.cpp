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



static esp_netif_t *_netif = NULL;

static int _reconnectAttempts = 0;
static WifiConfig cfg;



static void _on_wifi_sta_disconnect(void *arg, esp_event_base_t event_base,
							   int32_t event_id, void *event_data)
{
	_reconnectAttempts++;

	if (_reconnectAttempts > CONFIG_EXAMPLE_WIFI_CONN_MAX_RETRY) {
		ESP_LOGI(TAG, "WiFi Connect failed %d times, stop reconnect.", _reconnectAttempts);
		global::getEventManager()->pushEvent({event::EventGroup::WIFI, static_cast<int>(WifiEventId::FAILED_TO_CONNECT), NULL});

		return;
	}
	
	ESP_LOGI(TAG, "Wi-Fi disconnected, trying to reconnect...");
	//TODO: return assert, now it crashes at esp_restart()
	// ESP_ERROR_CHECK(esp_wifi_connect());
	esp_wifi_connect();
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


static void _on_wifi_ap_disconnect(void *arg, esp_event_base_t event_base,
							   int32_t event_id, void *event_data)
{
	// if (_reconnectAttempts > CONFIG_EXAMPLE_WIFI_CONN_MAX_RETRY) {
	// 	ESP_LOGI(TAG, "WiFi Connect failed %d times, stop reconnect.", _reconnectAttempts);
	// 	global::getEventManager()->pushEvent({event::EventGroup::WIFI, static_cast<int>(WifiEventId::FAILED_TO_CONNECT), NULL});

	// 	return;
	// }

	wifi_event_ap_stadisconnected_t *event = (wifi_event_ap_stadisconnected_t*) event_data;

	ESP_LOGI(TAG, "Wi-Fi sta disconnected: AID = %d", event->aid);

	global::getEventManager()->pushEvent({event::EventGroup::WIFI, static_cast<int>(WifiEventId::DISCONNECTED), NULL});

}

static void _on_ap_gave_ip(void *arg, esp_event_base_t event_base,
					  int32_t event_id, void *event_data)
{
	ip_event_ap_staipassigned_t *event = (ip_event_ap_staipassigned_t*) event_data;

	uint16_t aid = 0;

	esp_wifi_ap_get_sta_aid(event->mac, &aid);

	ESP_LOGI(TAG, "Provided IPv4, event: Interface \"%s\", AID = %d, address: " IPSTR, esp_netif_get_desc(event->esp_netif), aid, IP2STR(&event->ip));

	global::getEventManager()->pushEvent({event::EventGroup::WIFI, static_cast<int>(WifiEventId::CONNECTED), NULL});
}



// static esp_err_t _sta_disconnect()
// {
// 	ESP_LOGI(TAG, "%s", __FUNCTION__);

// 	ESP_ERROR_CHECK(esp_event_handler_unregister(WIFI_EVENT, WIFI_EVENT_STA_DISCONNECTED, &_on_wifi_sta_disconnect));
// 	ESP_ERROR_CHECK(esp_event_handler_unregister(IP_EVENT, IP_EVENT_STA_GOT_IP, &_on_sta_got_ip));
// 	ESP_ERROR_CHECK(esp_event_handler_unregister(WIFI_EVENT, WIFI_EVENT_STA_CONNECTED, &_on_wifi_connect));
// 	return esp_wifi_disconnect();
// }

// static bool _is_wifi_init()
// {
// 	wifi_mode_t mode;
// 	return ESP_ERR_WIFI_NOT_INIT != esp_wifi_get_mode(&mode);
// }


// static void _stop()
// {
// 	ESP_LOGI(TAG, "%s", __FUNCTION__);

// 	esp_err_t err = esp_wifi_stop();
// 	if (err == ESP_ERR_WIFI_NOT_INIT) {
// 		return;
// 	}
// 	ESP_ERROR_CHECK(err);
// 	// ESP_ERROR_CHECK(esp_wifi_deinit());
// 	ESP_ERROR_CHECK(esp_wifi_clear_default_wifi_driver_and_handlers(_netif));
// 	esp_netif_destroy(_netif);
// 	_netif = NULL;
// }



static void _init()
{
	ESP_ERROR_CHECK(esp_netif_init());
	ESP_ERROR_CHECK(esp_event_loop_create_default());

	wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
	ESP_ERROR_CHECK(esp_wifi_init(&cfg));
	ESP_ERROR_CHECK(esp_wifi_set_storage(WIFI_STORAGE_RAM));
}



static bool _checkWifiConfig(const WifiConfig& cfg)
{
	if(cfg.apSsid.length() < 8){
		FB_DEBUG_LOG_E_TAG("Invalid AP ssid length must be >= 8, %s", cfg.apSsid.c_str());
		return false;
	}

	if(cfg.staSsid.length() < 8){
		FB_DEBUG_LOG_E_TAG("Invalid STA ssid length must be >= 8, %s", cfg.staSsid.c_str());
		return false;
	}

	if(cfg.apPass.length() > 0 && cfg.apPass.length() < 8){
		FB_DEBUG_LOG_E_TAG("Invalid AP pass length must be >= 8 or 0");
		return false;
	}

	if(cfg.staPass.length() > 0 && cfg.staPass.length() < 8){
		FB_DEBUG_LOG_E_TAG("Invalid AP pass length must be >= 8 or 0");
		return false;
	}

	if(cfg.state != WifiState::AP && cfg.state != WifiState::STA){
		FB_DEBUG_LOG_E_TAG("Invalid wifi mode for initialization: %d", static_cast<int>(cfg.state));
		return false;
	}

	return true;
}

static void _launchWifi(const WifiConfig& cfg)
{
	if(cfg.state == WifiState::STA){
		ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_STA));
		_netif = esp_netif_create_default_wifi_sta();

		ESP_ERROR_CHECK(esp_event_handler_register(WIFI_EVENT, WIFI_EVENT_STA_CONNECTED, &_on_wifi_connect, NULL));
		ESP_ERROR_CHECK(esp_event_handler_register(WIFI_EVENT, WIFI_EVENT_STA_DISCONNECTED, &_on_wifi_sta_disconnect, NULL));
		ESP_ERROR_CHECK(esp_event_handler_register(IP_EVENT, IP_EVENT_STA_GOT_IP, &_on_sta_got_ip, NULL));

	}else if(cfg.state == WifiState::AP){
		ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_AP));
		_netif = esp_netif_create_default_wifi_ap();

		ESP_ERROR_CHECK(esp_event_handler_register(WIFI_EVENT, WIFI_EVENT_AP_STADISCONNECTED, &_on_wifi_ap_disconnect, NULL));
		ESP_ERROR_CHECK(esp_event_handler_register(IP_EVENT, IP_EVENT_AP_STAIPASSIGNED, &_on_ap_gave_ip, NULL));

	}else{
		assert(0);
	}

	ESP_ERROR_CHECK(esp_wifi_start());
}


bool wifi::operator==(int val, WifiEventId id)
{
	return val == static_cast<int>(id);
}



bool wifi::init()
{
	FB_DEBUG_ENTER_I_TAG();

	static std::once_flag flag;
	std::call_once(flag, &_init);

	return true;
}

bool wifi::start(const WifiConfig& cfg)
{
	FB_DEBUG_ENTER_I_TAG();

	esp_err_t result = ESP_OK;

	if(!_checkWifiConfig(cfg)){
		return false;
	}

	_launchWifi(cfg);

	//TODO: use state pattern
	if(cfg.state == WifiState::STA){
		//TODO: check current state of wifi, maybe need only to reconnect not full initialization
		_reconnectAttempts = 0;

		wifi_config_t wifi_config;
		memset(&wifi_config, 0, sizeof(wifi_config));

		memcpy(wifi_config.sta.ssid, cfg.staSsid.c_str(), cfg.staSsid.length() + 1);
		memcpy(wifi_config.sta.password, cfg.staPass.c_str(), cfg.staPass.length() + 1);

		ESP_ERROR_CHECK(esp_wifi_set_config(WIFI_IF_STA, &wifi_config));
		result = esp_wifi_connect();

	}else if(cfg.state == WifiState::AP){
		wifi_config_t wifi_config;
		memset(&wifi_config, 0, sizeof(wifi_config));

		memcpy(wifi_config.ap.ssid, cfg.apSsid.c_str(), cfg.apSsid.length() + 1);
		memcpy(wifi_config.ap.password, cfg.apPass.c_str(), cfg.apPass.length() + 1);

		wifi_config.ap.authmode = cfg.apPass.empty() ? WIFI_AUTH_OPEN : WIFI_AUTH_WPA2_PSK;
		wifi_config.ap.max_connection = 1;

		ESP_ERROR_CHECK(esp_wifi_set_config(WIFI_IF_AP, &wifi_config));

	}else{
		assert(0);
	}

	return result == ESP_OK;
}
