#include "fb_rfids_settings.hpp"

#include <cstring>
#include <cstdlib>

#include "fb_settings.hpp"



#define _PARTITION "prj_set"


#define _KEY_URL "http_url"

#define _KEY_LOG "log_en"

#define _KEY_READTIMEMS "read_ms"





using namespace fb;
using namespace settings;



/* ---------- setters ---------- */

void settings::setUrl(std::string value)
{
    
    settings::setStr(_PARTITION, _KEY_URL, value);
	
}

void settings::setLog(int value)
{
    
    settings::setInt(_PARTITION, _KEY_LOG, value);
    
}

void settings::setReadTimeMs(int value)
{
    
    settings::setInt(_PARTITION, _KEY_READTIMEMS, value);
    
}




/* ---------- getters ---------- */

std::string settings::getUrl()
{
    
    return settings::getStrOrDefault(_PARTITION, _KEY_URL, "http://192.168.0.111:8080");
    
}

int settings::getLog()
{
    
    return settings::getIntOrDefault(_PARTITION, _KEY_LOG, 1);
    
}

int settings::getReadTimeMs()
{
    
    return settings::getIntOrDefault(_PARTITION, _KEY_READTIMEMS, 160);
    
}
