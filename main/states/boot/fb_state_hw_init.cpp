#include "fb_state_hw_init.hpp"

#include "fb_core.hpp"
#include "fb_file_system.hpp"
#include "fb_globals.hpp"
#include "fb_sensor_event.hpp"
#include "fb_settings.hpp"
#include "fb_state_provision.hpp"
#include "fb_state_wifi_init.hpp"
#include "fb_wifi.hpp"



using namespace fb;
using namespace state;



StateHwInit::StateHwInit(StateManager& context)
	: State(context)
{

}

const char* StateHwInit::getName() const
{
	return "StateHwInit";
}

void StateHwInit::handleEvent(const event::Event& event)
{
	if(project::getInfo().requireSensorService){
		if(event.groupId == event::EventGroup::SENSOR){
			if(event.eventId == sensor::SensorEvent::ALL_SENSORS_INIT){
				if(settings::isWifiProvided()){
					getContext().transition(std::make_unique<StateWifiInit>(getContext()));
				}else{
					getContext().transition(std::make_unique<StateProvision>(getContext()));
				}
			}
		}
	}else{
		if(event.groupId == event::EventGroup::BOOT){
			if(settings::isWifiProvided()){
				getContext().transition(std::make_unique<StateWifiInit>(getContext()));
			}else{
				getContext().transition(std::make_unique<StateProvision>(getContext()));
			}
		}
	}
}

void StateHwInit::enter()
{
	fs::init();
	if(project::getInfo().requireSensorService){
		global::getSensorService()->start();
	}
	if(project::getInfo().requireSwitchService){
		global::getSwitchService()->start();
	}
	assert(wifi::init());

	// getContext().transition(std::make_unique<StateProvision>(getContext()));
}

void StateHwInit::exit()
{

}
