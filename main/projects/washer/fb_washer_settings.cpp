#include "fb_washer_settings.hpp"

#include <cstring>
#include <cstdlib>

#include "fb_settings.hpp"



#define _PARTITION "prj_set"


#define _KEY_URL "http_url"

#define _KEY_BOUNCETIMEMS "bounce"





using namespace fb;
using namespace settings;



/* ---------- setters ---------- */

void settings::setUrl(std::string value)
{
    
    settings::setStr(_PARTITION, _KEY_URL, value);
	
}

void settings::setBounceTimeMs(int value)
{
    
    settings::setInt(_PARTITION, _KEY_BOUNCETIMEMS, value);
    
}




/* ---------- getters ---------- */

std::string settings::getUrl()
{
    
    return settings::getStrOrDefault(_PARTITION, _KEY_URL, "http://192.168.0.111:8080");
    
}

int settings::getBounceTimeMs()
{
    
    return settings::getIntOrDefault(_PARTITION, _KEY_BOUNCETIMEMS, 50);
    
}
