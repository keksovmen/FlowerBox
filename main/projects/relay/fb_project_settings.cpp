#include "fb_relay_settings.hpp"

#include <cstring>
#include <cstdlib>

#include "fb_settings.hpp"



#define _PARTITION "prj_set"


#define _KEY_HTTP_DELAY_MS "h_delay"
#define _KEY_HTTP_URL "h_url"

#define _DEFAULT_HTTP_PULLER_URL "https://gameofmind.ru/_projects/gameofmind_deti/test/lights/lights.json"



using namespace fb;
using namespace settings;



void settings::setHttpDelay(int ms)
{
	settings::setInt(_PARTITION, _KEY_HTTP_DELAY_MS, ms);
}

void settings::setHttpUrl(std::string_view url)
{
	settings::setStr(_PARTITION, _KEY_HTTP_URL, url);
}



int settings::getHttpDelay()
{
	return settings::getIntOrDefault(_PARTITION, _KEY_HTTP_DELAY_MS, 300);
}

std::string settings::getHttpUrl()
{
	return settings::getStrOrDefault(_PARTITION, _KEY_HTTP_URL, _DEFAULT_HTTP_PULLER_URL);
}
