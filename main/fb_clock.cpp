#include "fb_clock.hpp"

#include <stdlib.h>
#include <string.h>
#include <sys/time.h>
#include <time.h>

#include "fb_debug.hpp"
#include "fb_globals.hpp"
#include "fb_settings.hpp"

#include "esp_log.h"
#include "esp_netif_sntp.h"
#include "esp_sntp.h"
#include "lwip/ip_addr.h"



using namespace fb;
using namespace clock;



static const char* TAG = "fb_clock";



static std::string _serverAddress;



static void _print_servers(void)
{
	FB_DEBUG_LOG_I_TAG("List of configured NTP servers:");

	for (uint8_t i = 0; i < SNTP_MAX_SERVERS; ++i){
		if (esp_sntp_getservername(i)){
			FB_DEBUG_LOG_I_TAG("server %d: %s", i, esp_sntp_getservername(i));
		} else {
			// we have either IPv4 or IPv6 address, let's print it
			char buff[128];
			ip_addr_t const *ip = esp_sntp_getserver(i);
			if (ipaddr_ntoa_r(ip, buff, sizeof(buff)) != NULL)
				FB_DEBUG_LOG_I_TAG("server %d: %s", i, buff);
		}
	}
}



static void _on_sntp_sync_event(struct timeval *tv)
{
	FB_DEBUG_ENTER_I_TAG();

	FB_DEBUG_LOG_W_TAG("Current time: %lld, or %lld", currentTimeStamp(), tv->tv_sec);

	global::getEventManager()->pushEvent({event::EventGroup::CLOCK, std::to_underlying(clock::ClockEventId::SYNCED), nullptr});
}



bool clock::operator==(int val, ClockEventId id)
{
	return id == static_cast<ClockEventId>(val);
}



Time::Time(Timestamp secs)
	: hours(secs / 3600),
		minutes((secs - (hours * 3600)) / 60),
		seconds(secs - (hours * 3600 + minutes * 60))
{

}

Time::Time(int hours, int minutes, int seconds)
	: hours(hours), minutes(minutes), seconds(seconds)
{

}

bool Time::operator<(const Time& t) const
{
	if(this->hours > t.hours){
		return false;
	}

	if(this->hours < t.hours){
		return true;
	}

	if(this->minutes > t.minutes){
		return false;
	}

	if(this->minutes < t.minutes){
		return true;
	}

	if(this->seconds > t.seconds){
		return false;
	}

	if(this->seconds < t.seconds){
		return true;
	}

	return false;
}

Time::operator Timestamp() const
{
	return this->hours * 60 * 60 + this->minutes * 60 + this->seconds;
}



void clock::initClock()
{
	FB_DEBUG_ENTER_I_TAG();

	//set current locale
	setenv("TZ", "GMT-3", 1);
	tzset();

	if(settings::getWifiMode() == settings::WifiMode::STA){
		_serverAddress = settings::getSntpServerUrl();
	}else{
		_serverAddress = "192.168.4.2";
	}

	assert(!_serverAddress.empty());

	esp_sntp_config_t config = ESP_NETIF_SNTP_DEFAULT_CONFIG(_serverAddress.c_str());
	config.wait_for_sync = false;
	config.start = false;
	config.sync_cb = &_on_sntp_sync_event;
	esp_netif_sntp_init(&config);

	_print_servers();
}

void clock::syncRequest()
{
	FB_DEBUG_ENTER_I_TAG();

	esp_netif_sntp_start();
}

void clock::deinitClock()
{
	FB_DEBUG_ENTER_I_TAG();

	esp_netif_sntp_deinit();
}

Time clock::getCurrentTime()
{
	time_t now;
	time(&now);

	struct tm tmp;
	localtime_r(&now, &tmp);

	return Time(tmp.tm_hour, tmp.tm_min, tmp.tm_sec);
}

Timestamp clock::currentTimeStamp()
{
	time_t now;
	time(&now);

	return now;
}
