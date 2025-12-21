#include "fb_relay_settings.hpp"

#include <cstring>
#include <cstdlib>

#include "fb_settings.hpp"



#define _PARTITION "prj_set"


#define _KEY_HTTPURL "h_url"

#define _KEY_HTTPDELAY "h_delay"





using namespace fb;
using namespace settings;



/* ---------- setters ---------- */

void settings::setHttpUrl(std::string value)
{
    
    settings::setStr(_PARTITION, _KEY_HTTPURL, value);
	
}

void settings::setHttpDelay(int value)
{
    
    settings::setInt(_PARTITION, _KEY_HTTPDELAY, value);
    
}




/* ---------- getters ---------- */

std::string settings::getHttpUrl()
{
    
    return settings::getStrOrDefault(_PARTITION, _KEY_HTTPURL, "https://192.168.1.50/mini_games/lights/lights.json");
    
}

int settings::getHttpDelay()
{
    
    return settings::getIntOrDefault(_PARTITION, _KEY_HTTPDELAY, 300);
    
}
