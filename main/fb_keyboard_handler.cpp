#include "fb_keyboard_handler.hpp"

#include "fb_settings.hpp"



using namespace fb;
using namespace keyboard;



const char* KeyboardHandler::getName() const
{
	return "KeyboardHandler";
}

void KeyboardHandler::handleEvent(const event::Event& event)
{
	if(event.groupId != event::EventGroup::KEYBOARD){
		return;
	}

	h::ButtonAction* action = static_cast<h::ButtonAction*>(event.data);
	_handleButtonAction(*action);
}

void KeyboardHandler::_handleButtonAction(const h::ButtonAction& action)
{
	// FB_DEBUG_LOG_I_OBJ("Button action: %d, %d, %lu", action.button, action.movement, action.holdMs);

	if(action.isLongJustPressed(h::ButtonKeys::RESET, 2000)){
		FB_DEBUG_LOG_I_OBJ("Button action RESET LONG -> dropping WIFI settings");

		//drop settings
		settings::clearWifi();
		//reboot???
		// global::getTimeScheduler()->addActionDelayed([](){esp_restart();}, 1000, portMAX_DELAY);
	}
}
