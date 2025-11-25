#include "fb_tressure_settings.hpp"

#include <cstring>
#include <cstdlib>

#include "fb_settings.hpp"



#define _PARTITION "prj_set"


#define _KEY_PULSEDURATION "f_pulse"

#define _KEY_BOOSTDELAY "f_boost"

#define _KEY_MQTTID "l_mqtt_id"





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
