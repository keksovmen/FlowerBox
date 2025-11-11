#include "fb_provision.hpp"

#include <utility>
#include <cstring>

#include <cJSON.h>
#include <wifi_provisioning/manager.h>
#include <wifi_provisioning/scheme_softap.h>
#include "esp_event.h"
#ifndef _ESP8266
	#include "esp_netif_types.h"
	#include "esp_wifi_default.h"
#endif
#include "esp_wifi.h"

#include "fb_debug.hpp"
#include "fb_globals.hpp"
#include "fb_settings.hpp"



#define _ENDPOINT_WIFI_MODE "box_settings"

#define _JSON_KEY_WIFI_MODE "wifi_mode"



using namespace fb;
using namespace provision;



static const char* TAG = "fb_provision";



#ifndef _ESP8266
	static esp_netif_t* _staNetif = nullptr;
	static esp_netif_t* _apNetif = nullptr;
#endif


static void _eventHandler(void* arg, esp_event_base_t eventBase, int32_t eventId, void* eventData)
{
	FB_DEBUG_LOG_I_TAG("Provision event %d", eventId);

	if(eventId == static_cast<int32_t>(WIFI_PROV_CRED_RECV))
	{
		wifi_sta_config_t* cfg = reinterpret_cast<wifi_sta_config_t*>(eventData);

		if(settings::getWifiMode() == settings::WifiMode::AP)
		{
			settings::setWifiMode(settings::WifiMode::AP);
			settings::setApSsid(reinterpret_cast<char*>(cfg->ssid));
			settings::setApPass(reinterpret_cast<char*>(cfg->password));
		}else
		{
			settings::setWifiMode(settings::WifiMode::STA);
			settings::setStaSsid(reinterpret_cast<char*>(cfg->ssid));
			settings::setStaPass(reinterpret_cast<char*>(cfg->password));
		}
	}
	else if(eventId == static_cast<int32_t>(WIFI_PROV_CRED_FAIL))
	{
		if(settings::getWifiMode() == settings::WifiMode::AP){
			//так и должно быть, можно закрывать наш менеджер
			global::getEventManager()->pushEvent({event::EventGroup::PROVISION, static_cast<int>(ProvisionEventId::SUCCESS), nullptr});	
		}
	}
	else if(eventId == static_cast<int32_t>(WIFI_PROV_CRED_SUCCESS))
	{
		global::getEventManager()->pushEvent({event::EventGroup::PROVISION, static_cast<int>(ProvisionEventId::SUCCESS), nullptr});
	}
}

static esp_err_t _customProvisionHandler(uint32_t sessionId, const uint8_t* inBuf, ssize_t inLen,
			uint8_t** outBuf, ssize_t* outLen, void* arg)
{
	FB_DEBUG_ENTER_I_TAG();

	if(!inBuf){
		return ESP_FAIL;
	}

	FB_DEBUG_LOG_I_TAG("%.*s", inLen, reinterpret_cast<const char*>(inBuf));
	
	cJSON* root = cJSON_ParseWithLength(reinterpret_cast<const char*>(inBuf), inLen);
	if(root == NULL){
		FB_DEBUG_LOG_E_TAG("Invalid JSON");
		
		return ESP_FAIL;
	}

	/*
		{
			"wifi_mode" : int,	//0 - AP, 1 - STA
		}
	*/

	cJSON* item = cJSON_GetObjectItem(root, _JSON_KEY_WIFI_MODE);
	if(item == NULL){
		FB_DEBUG_LOG_W_TAG("NOT FOUND JSON KEY: " _JSON_KEY_WIFI_MODE);
	}else{
		int wifiMode = static_cast<int>(cJSON_GetNumberValue(item));
		FB_DEBUG_LOG_I_TAG("WIFI mode %d", wifiMode);

		if(wifiMode < 0 || wifiMode >= static_cast<int>(settings::WifiMode::MAX)){
			FB_DEBUG_LOG_E_TAG("WIFI mode ILLEGAL");
		}else{
			settings::setWifiMode(static_cast<settings::WifiMode>(wifiMode));
		}
	}

	cJSON_Delete(root);

	return ESP_OK;
}



static void _initWifi()
{
	#ifndef _ESP8266
		_staNetif = esp_netif_create_default_wifi_sta();
		_apNetif = esp_netif_create_default_wifi_ap();

		assert(_staNetif);
		assert(_apNetif);
	#endif
}

static void _initManager()
{
	ESP_ERROR_CHECK(esp_event_handler_register(WIFI_PROV_EVENT, ESP_EVENT_ANY_ID, &_eventHandler, nullptr));

	wifi_prov_mgr_config_t cfg = {
		.scheme = wifi_prov_scheme_softap,
		.scheme_event_handler = WIFI_PROV_EVENT_HANDLER_NONE,
		.app_event_handler = WIFI_PROV_EVENT_HANDLER_NONE,
	};
	//TODO: register for provision events and on stop change state
    ESP_ERROR_CHECK(wifi_prov_mgr_init(cfg));

    wifi_prov_mgr_endpoint_create(_ENDPOINT_WIFI_MODE);

	ESP_ERROR_CHECK(wifi_prov_mgr_start_provisioning(
		WIFI_PROV_SECURITY_0,
		nullptr,
		global::getDeviceName().c_str(),
		std::to_string(global::getUniqueId()).c_str()
	));

    wifi_prov_mgr_endpoint_register(_ENDPOINT_WIFI_MODE, &_customProvisionHandler, nullptr);
}

static void _deinitWifi()
{
	esp_wifi_disconnect();
	esp_wifi_stop();
	
	#ifndef _ESP8266
		esp_netif_destroy_default_wifi(_staNetif);
		esp_netif_destroy_default_wifi(_apNetif);
	#endif
}

static void _deinitManager()
{
	esp_event_handler_unregister(WIFI_PROV_EVENT, ESP_EVENT_ANY_ID, &_eventHandler);
	wifi_prov_mgr_deinit();
}



void provision::init()
{
	FB_DEBUG_ENTER_I_TAG();

	_initWifi();
	_initManager();
}

void provision::deinit()
{
	FB_DEBUG_ENTER_I_TAG();

	_deinitManager();
	_deinitWifi();
}
