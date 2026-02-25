#pragma once



#include "fb_event_manager.hpp"
#include "fb_debug.hpp"



namespace fb
{
	namespace pins
	{
		//dmx
		static constexpr int PIN_DMX_TX = 10;
		static constexpr int PIN_DMX_RX = 2;
		static constexpr int PIN_DMX_RTS = 3;

		//relay
		static constexpr int PIN_RELAY = 4;

		//keyboard
		static constexpr int PIN_KEYBOARD_RESET = 9;
	}
}