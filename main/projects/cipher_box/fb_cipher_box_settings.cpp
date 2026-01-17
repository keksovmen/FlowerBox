#include "fb_cipher_box_settings.hpp"

#include <cstring>
#include <cstdlib>

#include "fb_settings.hpp"



#define _PARTITION "prj_set"


#define _KEY_URL "http_url"

#define _KEY_DELTAERROR "adc_error"





using namespace fb;
using namespace settings;



/* ---------- setters ---------- */

void settings::setUrl(std::string value)
{
    
    settings::setStr(_PARTITION, _KEY_URL, value);
	
}

void settings::setDeltaError(int value)
{
    
    settings::setInt(_PARTITION, _KEY_DELTAERROR, value);
    
}




/* ---------- getters ---------- */

std::string settings::getUrl()
{
    
    return settings::getStrOrDefault(_PARTITION, _KEY_URL, "http://192.168.0.111:8080");
    
}

int settings::getDeltaError()
{
    
    return settings::getIntOrDefault(_PARTITION, _KEY_DELTAERROR, 15);
    
}
