#include "fb_state_provision.hpp"

#include <cstring>

#include "fb_settings.hpp"
#include "fb_globals.hpp"
#include "fb_state_wifi_init.hpp"

#include <wifi_provisioning/manager.h>
#include <wifi_provisioning/scheme_softap.h>
#include "esp_event.h"
#include "esp_wifi_default.h"
#include "esp_wifi.h"



#define _ENDPOINT_WIFI_MODE "wifi_mode"



using namespace fb;
using namespace state;



StateProvision::StateProvision(StateManager& context)
	: State(context)
{

}

const char* StateProvision::getName()
{
	return "StateProvision";
}

void StateProvision::handleEvent(const event::Event& event)
{
	if(event.groupId == event::EventGroup::PROVISION){
		if(event.eventId == ProvisionEventId::SUCCESS){
			getContext().transition(std::make_unique<StateWifiInit>(getContext()));
		}
	}
}

void StateProvision::enter()
{
	_initWifi();
	_initProvision();
}

void StateProvision::exit()
{
	_deinitProvision();
	_deinitWifi();
}

void StateProvision::_eventHandler(void* arg, esp_event_base_t eventBase, int32_t eventId, void* eventData)
{
	StateProvision* me = reinterpret_cast<StateProvision*>(arg);

	FB_DEBUG_PTR_LOG(me, "Provision event %ld", eventId);

	if(eventId == static_cast<int32_t>(WIFI_PROV_CRED_SUCCESS)){
		global::getEventManager()->pushEvent({event::EventGroup::PROVISION, static_cast<int>(ProvisionEventId::SUCCESS), nullptr});
	}
}

esp_err_t StateProvision::_customProvisionHandler(uint32_t sessionId, const uint8_t* inBuf, ssize_t inLen,
							uint8_t** outBuf, ssize_t* outLen, void* arg)
{
	StateProvision* me = static_cast<StateProvision*>(arg);
	const char* TAG = me->getName();

	if(inBuf){
		FB_DEBUG_TAG_LOG("%.*s", inLen, reinterpret_cast<const char*>(inBuf));
	}

	*outBuf = reinterpret_cast<uint8_t*>(strdup("some response"));
	*outLen = strlen(reinterpret_cast<char*>(*outBuf)) + 1;

	//TODO: set wifi mode

	return ESP_OK;
}



void StateProvision::_initWifi()
{
	_staNetif = esp_netif_create_default_wifi_sta();
    _apNetif = esp_netif_create_default_wifi_ap();

	assert(_staNetif);
	assert(_apNetif);
}

void StateProvision::_initProvision()
{
	ESP_ERROR_CHECK(esp_event_handler_register(WIFI_PROV_EVENT, ESP_EVENT_ANY_ID, &StateProvision::_eventHandler, this));

	wifi_prov_mgr_config_t cfg = {
		.scheme = wifi_prov_scheme_softap,
		.scheme_event_handler = WIFI_PROV_EVENT_HANDLER_NONE,
		.app_event_handler = WIFI_PROV_EVENT_HANDLER_NONE,
	};
	//TODO: register for provision events and on stop change state
    ESP_ERROR_CHECK(wifi_prov_mgr_init(cfg));

    wifi_prov_mgr_endpoint_create(_ENDPOINT_WIFI_MODE);

	std::string deviceName = "flowerbox_" + std::to_string(global::getUniqueId());
	ESP_ERROR_CHECK(wifi_prov_mgr_start_provisioning(WIFI_PROV_SECURITY_0, nullptr, deviceName.c_str(), nullptr));

    wifi_prov_mgr_endpoint_register(_ENDPOINT_WIFI_MODE, &StateProvision::_customProvisionHandler, this);
}

void StateProvision::_deinitWifi()
{
	esp_wifi_disconnect();
	esp_wifi_stop();
	
	esp_netif_destroy_default_wifi(_staNetif);
	esp_netif_destroy_default_wifi(_apNetif);
}

void StateProvision::_deinitProvision()
{
	esp_event_handler_unregister(WIFI_PROV_EVENT, ESP_EVENT_ANY_ID, &StateProvision::_eventHandler);
	wifi_prov_mgr_deinit();
}
