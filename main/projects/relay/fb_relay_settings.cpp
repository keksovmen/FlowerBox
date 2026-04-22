#include "fb_relay_settings.hpp"

#include <cstring>
#include <cstdlib>

#include "fb_settings.hpp"



#define _PARTITION "prj_set"


#define _KEY_HTTPURL "h_url"

#define _KEY_HTTPDELAY "h_delay"

#define _KEY_MQTTID "l_mqtt_id"

#define _KEY_IP "mq_ip"

#define _KEY_PORT "mq_port"





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

std::string settings::getHttpUrl()
{
    
    return settings::getStrOrDefault(_PARTITION, _KEY_HTTPURL, "https://192.168.1.50/mini_games/lights/lights.json");
    
}

int settings::getHttpDelay()
{
    
    return settings::getIntOrDefault(_PARTITION, _KEY_HTTPDELAY, 300);
    
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
