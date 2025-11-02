#pragma once



#include <array>

#include "fb_event_manager.hpp"
#include "fb_debug.hpp"



namespace fb
{
	namespace pins
	{
		static constexpr int RELAY_PINS_COUNT = 6;

		//keyboard
		static constexpr int PIN_KEYBOARD_RESET = 9;

		//relay
		static constexpr int PIN_RELAY_1 = 4;
		static constexpr int PIN_RELAY_2 = 5;
		static constexpr int PIN_RELAY_3 = 6;
		static constexpr int PIN_RELAY_4 = 8;
		static constexpr int PIN_RELAY_5 = 11;
		static constexpr int PIN_RELAY_6 = 12;

		static constexpr std::array<int, PIN_KEYBOARD_RESET> PINS_RELAY({
			PIN_RELAY_1,
			PIN_RELAY_2,
			PIN_RELAY_3,
			PIN_RELAY_4,
			PIN_RELAY_5,
			PIN_RELAY_6
			});
	}
}