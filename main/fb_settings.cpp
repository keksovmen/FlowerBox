#include "fb_settings.hpp"

#include <utility>



#define _DEFAULT_WIFI_MODE WifiMode::STA
#define _DEFAULT_AP_SSID "TEST_FLOWER_BOX"
#define _DEFAULT_AP_PASS "12345678"
#define _DEFAULT_STA_SSID CONFIG_EXAMPLE_WIFI_SSID
#define _DEFAULT_STA_PASS CONFIG_EXAMPLE_WIFI_PASSWORD
#define _DEFAULT_URL_SNTP CONFIG_SNTP_TIME_SERVER

#define _PARTION_NET "net"

#define _KEY_WIFI_MODE "wifi_mode"
#define _KEY_WIFI_AP_SSID "ap_ssid"
#define _KEY_WIFI_AP_PASS "ap_pass"
#define _KEY_WIFI_STA_SSID "sta_pass"
#define _KEY_WIFI_STA_PASS "sta_pass"
#define _KEY_URL_SNTP "url_sntp"



using namespace fb;
using namespace settings;



static const char* TAG = "fb_settings";



static NetworkConfig _netConfig = {
	.mode = WifiMode::STA,
	.apSsid = "TEST_FLOWER_BOX",
	.apPass = "12345678",
	.staSsid = CONFIG_EXAMPLE_WIFI_SSID,
	.staPass = CONFIG_EXAMPLE_WIFI_PASSWORD,
};

static std::unique_ptr<storage::StorageIface> _storage;



static std::string _getStrOrDefault(const std::string& partition,
			const std::string& key,
			const std::string& def)
{
	if(!_storage){
		FB_DEBUG_TAG_LOG("Has no store: %s, ret default %s", key.c_str(), def.c_str());
		return def;
	}

	if(!_storage->hasKey(partition, key)){
		FB_DEBUG_TAG_LOG("Has no key: %s, ret default %s", key.c_str(), def.c_str());
		return def;
	}

	std::string result;
	if(!_storage->readValue(partition, key, result)){
		FB_DEBUG_TAG_LOG("Failed to read key: %s, ret default %s", key.c_str(), def.c_str());
		return def;
	}

	return result;
}

static int _getIntOrDefault(const std::string& partition,
			const std::string& key,
			int def)
{
	if(!_storage){
		FB_DEBUG_TAG_LOG("Has no store: %s, ret default %d", key.c_str(), def);
		return def;
	}

	if(!_storage->hasKey(partition, key)){
		FB_DEBUG_TAG_LOG("Has no key: %s, ret default %d", key.c_str(), def);
		return def;
	}

	int result;
	if(!_storage->readValue(partition, key, result)){
		FB_DEBUG_TAG_LOG("Failed to read key: %s, ret default %d", key.c_str(), def);
		return def;
	}

	return result;
}

static void _setStr(const std::string& partion,
			const std::string& key,
			const std::string& val)
{
	if(!_storage){
		FB_DEBUG_TAG_LOG_E("No storage");
		return;
	}

	if(!_storage->writeValue(partion, key, val)){
		FB_DEBUG_TAG_LOG_E("Failed to set key: %s, val: %s", key.c_str(), val.c_str());
		return;
	}

	FB_DEBUG_TAG_LOG("Set key: %s, val: %s", key.c_str(), val.c_str());
}

static void _setInt(const std::string& partion,
			const std::string& key,
			int val)
{
	if(!_storage){
		FB_DEBUG_TAG_LOG_E("No storage");
		return;
	}

	if(!_storage->writeValue(partion, key, val)){
		FB_DEBUG_TAG_LOG_E("Failed to set key: %s, val: %d", key.c_str(), val);
		return;
	}

	FB_DEBUG_TAG_LOG("Set key: %s, val: %d", key.c_str(), val);
}


void settings::init(std::unique_ptr<storage::StorageIface> storage)
{
	_storage = std::move(storage);
}

std::string settings::getApSsid()
{
	return _getStrOrDefault(_PARTION_NET, _KEY_WIFI_AP_SSID, _DEFAULT_AP_SSID);
}

std::string settings::getApPass()
{
	return _getStrOrDefault(_PARTION_NET, _KEY_WIFI_AP_PASS, _DEFAULT_AP_PASS);
}

std::string settings::getStaSsid()
{
	return _getStrOrDefault(_PARTION_NET, _KEY_WIFI_STA_SSID, _DEFAULT_STA_SSID);
}

std::string settings::getStaPass()
{
	return _getStrOrDefault(_PARTION_NET, _KEY_WIFI_STA_PASS, _DEFAULT_STA_PASS);
}

WifiMode settings::getWifiMode()
{
	return static_cast<WifiMode>(
		_getIntOrDefault(_PARTION_NET, _KEY_WIFI_MODE, std::to_underlying(_DEFAULT_WIFI_MODE)));
}

std::string settings::getSntpServerUrl()
{
	return _getStrOrDefault(_PARTION_NET, _KEY_URL_SNTP, _DEFAULT_URL_SNTP);
}

bool settings::isWifiProvided()
{
	return _storage &&
		_storage->hasKey(_PARTION_NET, _KEY_WIFI_MODE) &&
		_storage->hasKey(_PARTION_NET, _KEY_WIFI_STA_SSID) &&
		_storage->hasKey(_PARTION_NET, _KEY_WIFI_STA_PASS);
}

void settings::setApSsid(const std::string& val)
{
	_setStr(_PARTION_NET, _KEY_WIFI_AP_SSID, val);
}

void settings::setApPass(const std::string& val)
{
	_setStr(_PARTION_NET, _KEY_WIFI_AP_PASS, val);
}

void settings::setStaSsid(const std::string& val)
{
	_setStr(_PARTION_NET, _KEY_WIFI_STA_SSID, val);
}

void settings::setStaPass(const std::string& val)
{
	_setStr(_PARTION_NET, _KEY_WIFI_STA_PASS, val);
}

void settings::setWifiMode(WifiMode val)
{
	_setInt(_PARTION_NET, _KEY_WIFI_MODE, std::to_underlying(val));
}