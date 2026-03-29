#include "fb_washer_settings.hpp"

#include <cstring>
#include <cstdlib>

#include "fb_settings.hpp"



#define _PARTITION "prj_set"


#define _KEY_URL "http_url"

#define _KEY_BOUNCETIMEMS "bounce"

#define _KEY_MQTTID "l_mqtt_id"

#define _KEY_IP "mq_ip"

#define _KEY_PORT "mq_port"





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




/* ---------- getters ---------- */

std::string settings::getUrl()
{
    
    return settings::getStrOrDefault(_PARTITION, _KEY_URL, "http://192.168.0.111:8080");
    
}

int settings::getBounceTimeMs()
{
    
    return settings::getIntOrDefault(_PARTITION, _KEY_BOUNCETIMEMS, 50);
    
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
