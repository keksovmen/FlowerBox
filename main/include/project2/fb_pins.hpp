#pragma once



#include "fb_event_manager.hpp"
#include "fb_debug.hpp"



namespace fb
{
	namespace pins
	{
		//df player mini
		static constexpr int PIN_MP3_TX = 19;
		static constexpr int PIN_MP3_RX = 18;

		//dmx
		static constexpr int PIN_DMX_TX = 23;
		static constexpr int PIN_DMX_RX = 22;
		static constexpr int PIN_DMX_RTS = 21;

		//keyboard
		static constexpr int PIN_KEYBOARD_RESET = 0;
	}
}