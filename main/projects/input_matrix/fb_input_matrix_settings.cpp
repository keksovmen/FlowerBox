#include "fb_input_matrix_settings.hpp"

#include <cstring>
#include <cstdlib>

#include "fb_settings.hpp"



#define _PARTITION "prj_set"


#define _KEY_MQTTID "l_mqtt_id"

#define _KEY_IP "mq_ip"

#define _KEY_PORT "mq_port"

#define _KEY_PULSETIME "t_pulse"

#define _KEY_COMBINATION "comb"

#define _KEY_DEBOUNCEMS "t_deb"

#define _KEY_STILLPERIODMS "t_still"





using namespace fb;
using namespace settings;



/* ---------- setters ---------- */

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

void settings::setPulseTime(int value)
{
    
    settings::setInt(_PARTITION, _KEY_PULSETIME, value);
    
}

void settings::setCombination(std::string value)
{
    
    settings::setStr(_PARTITION, _KEY_COMBINATION, value);
	
}

void settings::setDebounceMs(int value)
{
    
    settings::setInt(_PARTITION, _KEY_DEBOUNCEMS, value);
    
}

void settings::setStillPeriodMs(int value)
{
    
    settings::setInt(_PARTITION, _KEY_STILLPERIODMS, value);
    
}




/* ---------- getters ---------- */

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

int settings::getPulseTime()
{
    
    return settings::getIntOrDefault(_PARTITION, _KEY_PULSETIME, 4);
    
}

std::string settings::getCombination()
{
    
    return settings::getStrOrDefault(_PARTITION, _KEY_COMBINATION, "1");
    
}

int settings::getDebounceMs()
{
    
    return settings::getIntOrDefault(_PARTITION, _KEY_DEBOUNCEMS, 15);
    
}

int settings::getStillPeriodMs()
{
    
    return settings::getIntOrDefault(_PARTITION, _KEY_STILLPERIODMS, 500);
    
}
