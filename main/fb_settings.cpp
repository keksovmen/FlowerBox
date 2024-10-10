#include "fb_settings.hpp"



using namespace fb;
using namespace settings;



static NetworkConfig _netConfig = {
	.mode = WifiMode::STA,
	.apSsid = "TEST_FLOWER_BOX",
	.apPass = "12345678",
	.staSsid = CONFIG_EXAMPLE_WIFI_SSID,
	.staPass = CONFIG_EXAMPLE_WIFI_PASSWORD,
};



std::string settings::getApSsid()
{
	return _netConfig.apSsid;
}

std::string settings::getApPass()
{
	return _netConfig.apPass;
}

std::string settings::getStaSsid()
{
	return _netConfig.staSsid;
}

std::string settings::getStaPass()
{
	return _netConfig.staPass;
}

WifiMode settings::getWifiMode()
{
	return _netConfig.mode;
}

std::string settings::getSntpServerUrl()
{
	return CONFIG_SNTP_TIME_SERVER;
}

bool settings::isWifiProvided()
{
	return false;
}

void settings::setApSsid(const std::string& val)
{
	_netConfig.apPass = val;
}

void settings::setApPass(const std::string& val)
{
	_netConfig.apPass = val;
}

void settings::setStaSsid(const std::string& val)
{
	_netConfig.staSsid = val;
}

void settings::setStaPass(const std::string& val)
{
	_netConfig.staPass = val;
}

void settings::setWifiMode(WifiMode val)
{
	_netConfig.mode = val;
}