#include "fb_settings.hpp"

#include <cstring>
#include <utility>



#define _DEFAULT_WIFI_MODE WifiMode::STA
#define _DEFAULT_AP_PASS "12345678"
#define _DEFAULT_AP_SSID "FlowerBox_" _DEFAULT_AP_PASS
#define _DEFAULT_STA_SSID CONFIG_EXAMPLE_WIFI_SSID
#define _DEFAULT_STA_PASS CONFIG_EXAMPLE_WIFI_PASSWORD
#define _DEFAULT_URL_SNTP CONFIG_SNTP_TIME_SERVER

#define _PARTION_NET "net"

#define _KEY_WIFI_MODE "wifi_mode"
#define _KEY_WIFI_AP_SSID "ap_ssid"
#define _KEY_WIFI_AP_PASS "ap_pass"
#define _KEY_WIFI_STA_SSID "sta_ssid"
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



void settings::init(std::unique_ptr<storage::StorageIface> storage)
{
	_storage = std::move(storage);
}

storage::StorageIface* getStorage()
{
	return _storage.get();
}

std::string settings::getStrOrDefault(
			std::string_view partition,
			std::string_view key,
			std::string_view def)
{
	if(!_storage){
		FB_DEBUG_LOG_I_TAG("Has no store: %s, ret default %s", key.cbegin(), def.cbegin());
		//TODO: remove explicit constructor
		return std::string{def};
	}

	//TODO: made hasKey accept string views
	if(!_storage->hasKey(partition, key)){
		FB_DEBUG_LOG_I_TAG("Has no key: %s, ret default %s", key.cbegin(), def.cbegin());
		return std::string{def};
	}

	//TODO: made readValue accept string views
	std::string result;
	if(!_storage->readValue(partition, key, result)){
		FB_DEBUG_LOG_I_TAG("Failed to read key: %s, ret default %s", key.cbegin(), def.cbegin());
		return std::string{def};
	}

	return result;
}

int64_t settings::getIntOrDefault(std::string_view partition,
			std::string_view key,
			int64_t def)
{
	if(!_storage){
		FB_DEBUG_LOG_I_TAG("Has no store: %s, ret default %lld", key.cbegin(), def);
		return def;
	}

	if(!_storage->hasKey(partition, key)){
		FB_DEBUG_LOG_I_TAG("Has no key: %s, ret default %lld", key.cbegin(), def);
		return def;
	}

	int64_t result;
	if(!_storage->readValue(partition, key, result)){
		FB_DEBUG_LOG_I_TAG("Failed to read key: %s, ret default %lld", key.cbegin(), def);
		return def;
	}

	return result;
}

float settings::getFloatOrDefault(std::string_view partition,
			std::string_view key,
			float def)
{
	auto result = getStrOrDefault(partition, key, "");
	if(result.empty()){
		return def;
	}

	return std::atof(result.c_str());
}

void settings::setStr(std::string_view partion,
			std::string_view key,
			std::string_view val)
{
	if(!_storage){
		FB_DEBUG_LOG_E_TAG("No storage");
		return;
	}

	if(!_storage->writeValue(partion, key, val)){
		FB_DEBUG_LOG_E_TAG("Failed to set key: %s, val: %s", key.cbegin(), val.cbegin());
		return;
	}

	FB_DEBUG_LOG_I_TAG("Set key: %s, val: %s", key.cbegin(), val.cbegin());
}

void settings::setInt(std::string_view partion,
			std::string_view key,
			int64_t val)
{
	if(!_storage){
		FB_DEBUG_LOG_E_TAG("No storage");
		return;
	}

	if(!_storage->writeValue(partion, key, val)){
		FB_DEBUG_LOG_E_TAG("Failed to set key: %s, val: %lld", key.cbegin(), val);
		return;
	}

	FB_DEBUG_LOG_I_TAG("Set key: %s, val: %lld", key.cbegin(), val);
}

void settings::setFloat(std::string_view partion,
			std::string_view key,
			float val)
{
	char tmp[16] = {0};
	sprintf(tmp, "%.6f", val);

	settings::setStr(partion, key, tmp);
}

std::string settings::getApSsid()
{
	return getStrOrDefault(_PARTION_NET, _KEY_WIFI_AP_SSID, _DEFAULT_AP_SSID);
}

std::string settings::getApPass()
{
	return getStrOrDefault(_PARTION_NET, _KEY_WIFI_AP_PASS, _DEFAULT_AP_PASS);
}

std::string settings::getStaSsid()
{
	return getStrOrDefault(_PARTION_NET, _KEY_WIFI_STA_SSID, _DEFAULT_STA_SSID);
}

std::string settings::getStaPass()
{
	return getStrOrDefault(_PARTION_NET, _KEY_WIFI_STA_PASS, _DEFAULT_STA_PASS);
}

WifiMode settings::getWifiMode()
{
	return static_cast<WifiMode>(
		getIntOrDefault(_PARTION_NET, _KEY_WIFI_MODE, std::to_underlying(_DEFAULT_WIFI_MODE)));
}

std::string settings::getSntpServerUrl()
{
	return getStrOrDefault(_PARTION_NET, _KEY_URL_SNTP, _DEFAULT_URL_SNTP);
}

bool settings::isWifiProvided()
{
	return _storage &&
		_storage->hasKey(_PARTION_NET, _KEY_WIFI_MODE) && (
		(_storage->hasKey(_PARTION_NET, _KEY_WIFI_STA_SSID) &&
		_storage->hasKey(_PARTION_NET, _KEY_WIFI_STA_PASS)) ||
		(_storage->hasKey(_PARTION_NET, _KEY_WIFI_AP_SSID) &&
		_storage->hasKey(_PARTION_NET, _KEY_WIFI_AP_SSID)));
}

void settings::setApSsid(const std::string& val)
{
	setStr(_PARTION_NET, _KEY_WIFI_AP_SSID, val);
}

void settings::setApPass(const std::string& val)
{
	setStr(_PARTION_NET, _KEY_WIFI_AP_PASS, val);
}

void settings::setStaSsid(const std::string& val)
{
	setStr(_PARTION_NET, _KEY_WIFI_STA_SSID, val);
}

void settings::setStaPass(const std::string& val)
{
	setStr(_PARTION_NET, _KEY_WIFI_STA_PASS, val);
}

void settings::setWifiMode(WifiMode val)
{
	setInt(_PARTION_NET, _KEY_WIFI_MODE, std::to_underlying(val));
}

void settings::clearWifi()
{
	_storage->clearValue(_PARTION_NET, _KEY_WIFI_MODE);

	_storage->clearValue(_PARTION_NET, _KEY_WIFI_STA_SSID);
	_storage->clearValue(_PARTION_NET, _KEY_WIFI_STA_PASS);
	
	_storage->clearValue(_PARTION_NET, _KEY_WIFI_AP_SSID);
	_storage->clearValue(_PARTION_NET, _KEY_WIFI_AP_PASS);
}