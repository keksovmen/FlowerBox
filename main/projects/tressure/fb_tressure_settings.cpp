#include "fb_tressure_settings.hpp"

#include <cstring>
#include <cstdlib>

#include "fb_settings.hpp"



#define _PARTITION "prj_set"


#define _KEY_PULSEDURATION "f_pulse"

#define _KEY_BOOSTDELAY "f_boost"

#define _KEY_MQTTID "l_mqtt_id"

#define _KEY_BATTERYMAX "b_max"

#define _KEY_BATTERYMIN "b_min"

#define _KEY_IP "mq_ip"

#define _KEY_PORT "mq_port"





using namespace fb;
using namespace settings;



/* ---------- setters ---------- */

void settings::setPulseDuration(int value)
{
    
    settings::setInt(_PARTITION, _KEY_PULSEDURATION, value);
    
}

void settings::setBoostDelay(int value)
{
    
    settings::setInt(_PARTITION, _KEY_BOOSTDELAY, value);
    
}

void settings::setMqttId(int value)
{
    
    settings::setInt(_PARTITION, _KEY_MQTTID, value);
    
}

void settings::setBatteryMax(int value)
{
    
    settings::setInt(_PARTITION, _KEY_BATTERYMAX, value);
    
}

void settings::setBatteryMin(int value)
{
    
    settings::setInt(_PARTITION, _KEY_BATTERYMIN, value);
    
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

int settings::getPulseDuration()
{
    
    return settings::getIntOrDefault(_PARTITION, _KEY_PULSEDURATION, 100);
    
}

int settings::getBoostDelay()
{
    
    return settings::getIntOrDefault(_PARTITION, _KEY_BOOSTDELAY, 100);
    
}

int settings::getMqttId()
{
    
    return settings::getIntOrDefault(_PARTITION, _KEY_MQTTID, 0);
    
}

int settings::getBatteryMax()
{
    
    return settings::getIntOrDefault(_PARTITION, _KEY_BATTERYMAX, 3150);
    
}

int settings::getBatteryMin()
{
    
    return settings::getIntOrDefault(_PARTITION, _KEY_BATTERYMIN, 2315);
    
}

std::string settings::getIp()
{
    
    return settings::getStrOrDefault(_PARTITION, _KEY_IP, "192.168.0.111");
    
}

int settings::getPort()
{
    
    return settings::getIntOrDefault(_PARTITION, _KEY_PORT, 8081);
    
}
