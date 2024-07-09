#include "fb_settings.hpp"



using namespace fb;
using namespace settings;



std::string settings::getApSsid()
{
	return "TEST_FLOWER_BOX";
}

std::string settings::getApPass()
{
	return "12345678";
}

std::string settings::getStaSsid()
{
	return CONFIG_EXAMPLE_WIFI_SSID;
}

std::string settings::getStaPass()
{
	return CONFIG_EXAMPLE_WIFI_PASSWORD;
}

WifiMode settings::getWifiMode()
{
	return WifiMode::STA;
}

std::string settings::getSntpServerUrl()
{
	return CONFIG_SNTP_TIME_SERVER;
}
