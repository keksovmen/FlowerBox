#include "fb_clock.hpp"

#include <string.h>
#include <time.h>
#include <sys/time.h>

#include "fb_debug.hpp"
#include "fb_globals.hpp"

#include "esp_log.h"
#include "esp_netif_sntp.h"
#include "esp_sntp.h"
#include "lwip/ip_addr.h"



using namespace fb;
using namespace clock;



static const char* TAG = "fb_clock";



static void _print_servers(void)
{
	FB_DEBUG_TAG_LOG("List of configured NTP servers:");

	for (uint8_t i = 0; i < SNTP_MAX_SERVERS; ++i){
		if (esp_sntp_getservername(i)){
			FB_DEBUG_TAG_LOG("server %d: %s", i, esp_sntp_getservername(i));
		} else {
			// we have either IPv4 or IPv6 address, let's print it
			char buff[128];
			ip_addr_t const *ip = esp_sntp_getserver(i);
			if (ipaddr_ntoa_r(ip, buff, sizeof(buff)) != NULL)
				FB_DEBUG_TAG_LOG("server %d: %s", i, buff);
		}
	}
}



static void _on_sntp_sync_event(struct timeval *tv)
{
	FB_DEBUG_TAG_ENTER();

	global::getEventManager()->pushEvent({event::EventGroup::CLOCK, static_cast<int>(clock::ClockEventId::SYNCED), nullptr});

	FB_DEBUG_TAG_EXIT();
}



bool clock::operator==(int val, ClockEventId id)
{
	return id == static_cast<ClockEventId>(val);
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



void clock::initClock()
{
	FB_DEBUG_TAG_ENTER();

	esp_sntp_config_t config = ESP_NETIF_SNTP_DEFAULT_CONFIG(CONFIG_SNTP_TIME_SERVER);
	config.wait_for_sync = false;
	config.start = false;
	config.sync_cb = &_on_sntp_sync_event;
	esp_netif_sntp_init(&config);

	_print_servers();

	FB_DEBUG_TAG_EXIT();
}

void clock::syncRequest()
{
	FB_DEBUG_TAG_ENTER();

	esp_netif_sntp_start();

	FB_DEBUG_TAG_EXIT();
}

void clock::deinitClock()
{
	FB_DEBUG_TAG_ENTER();

	esp_netif_sntp_deinit();

	FB_DEBUG_TAG_EXIT();
}
