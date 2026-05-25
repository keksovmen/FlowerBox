#include "fb_cipher_box_settings.hpp"

#include <cstring>
#include <cstdlib>

#include "fb_settings.hpp"



#define _PARTITION "prj_set"


#define _KEY_URL "http_url"

#define _KEY_DELTAERROR "adc_error"

#define _KEY_BOUNCETIMEMS "adc_bounce"

#define _KEY_CHIPSCOUNT "chips"

#define _KEY_READINGSCOUNT "readings"

#define _KEY_MQTTID "l_mqtt_id"

#define _KEY_IP "mq_ip"

#define _KEY_PORT "mq_port"

#define _KEY_USEHTTP "http_en"





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

void settings::setBounceTimeMs(int value)
{
    
    settings::setInt(_PARTITION, _KEY_BOUNCETIMEMS, value);
    
}

void settings::setChipsCount(int value)
{
    
    settings::setInt(_PARTITION, _KEY_CHIPSCOUNT, value);
    
}

void settings::setReadingsCount(int value)
{
    
    settings::setInt(_PARTITION, _KEY_READINGSCOUNT, value);
    
}

void settings::setMqttId(int value)
{
    
    settings::setInt(_PARTITION, _KEY_MQTTID, value);
    
}

void settings::setIp(std::string value)
{
    
    settings::setStr(_PARTITION, _KEY_IP, value);
	
}

void settings::setPort(int value)
{
    
    settings::setInt(_PARTITION, _KEY_PORT, value);
    
}

void settings::setUseHttp(int value)
{
    
    settings::setInt(_PARTITION, _KEY_USEHTTP, value);
    
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

int settings::getBounceTimeMs()
{
    
    return settings::getIntOrDefault(_PARTITION, _KEY_BOUNCETIMEMS, 500);
    
}

int settings::getChipsCount()
{
    
    return settings::getIntOrDefault(_PARTITION, _KEY_CHIPSCOUNT, 1);
    
}

int settings::getReadingsCount()
{
    
    return settings::getIntOrDefault(_PARTITION, _KEY_READINGSCOUNT, 4);
    
}

int settings::getMqttId()
{
    
    return settings::getIntOrDefault(_PARTITION, _KEY_MQTTID, 0);
    
}

std::string settings::getIp()
{
    
    return settings::getStrOrDefault(_PARTITION, _KEY_IP, "192.168.0.111");
    
}

int settings::getPort()
{
    
    return settings::getIntOrDefault(_PARTITION, _KEY_PORT, 8081);
    
}

int settings::getUseHttp()
{
    
    return settings::getIntOrDefault(_PARTITION, _KEY_USEHTTP, 0);
    
}
