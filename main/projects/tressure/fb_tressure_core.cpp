#include "fb_core.hpp"

#include "fb_globals.hpp"
#include "fb_sleep.hpp"
#include "fb_tressure_box_obj.hpp"
#include "fb_tressure_hw_obj.hpp"
#include "fb_tressure_mapper_obj.hpp"
#include "fb_tressure_pins.hpp"
#include "fb_wifi.hpp"
#include "fb_sensor_keyboard.hpp"



using namespace fb;
using namespace project;



static void _handleEvent(const event::Event& event);
 


static BaseCore _core{&_handleEvent, "TressureBox"};



static CoreInfo _info = {
	.core = _core,
	.requiresTime = false,
	.requireSensorService = true,
	.requireSwitchService = false,
};



static void _handleEvent(const event::Event& event)
{
	//do stuff
	if(event.groupId == event::EventGroup::WIFI && event.eventId == wifi::WifiEventId::CONNECTED){
		// ESP_ERROR_CHECK(esp_wifi_set_protocol(WIFI_IF_STA, WIFI_PROTOCOL_11N));
		sleep::enableLightSleep();
		global::getTimeScheduler()->addActionDelayed([](){project::forceStatusPost();}, 1000, 1000);
	}else if(event.groupId == event::EventGroup::KEYBOARD){
		h::ButtonAction* action = static_cast<h::ButtonAction*>(event.data);
		if(action->isJustPressed(h::ButtonKeys::MODE)){
			//tell HW that we are closed
			project::doorIsOpened();
		}else if(action->button == h::ButtonKeys::MODE && (action->movement == h::ButtonMovement::PRESSED || action->movement == h::ButtonMovement::STILL_PRESSED)){
			//tell HW that we are open
			project::doorIsClosed();
		}
	}else if(event.groupId == event::EventGroup::SENSOR && event.eventId == sensor::SensorEvent::ALL_SENSORS_INIT){
		bool door = gpio_get_level(static_cast<gpio_num_t>(pins::PIN_LOCK_SENSOR));
		if(door){
			doorIsOpened();
		}else{
			doorIsClosed();
		}
	}
}







void project::bootInit()
{
	
}

const CoreInfo& project::getInfo()
{
	return _info;
}