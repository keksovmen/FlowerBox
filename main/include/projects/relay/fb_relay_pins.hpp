#pragma once



#include <array>



namespace fb
{
	namespace pins
	{
		static constexpr int RELAY_PINS_COUNT = 10;

		//keyboard
		static constexpr int PIN_KEYBOARD_RESET = 9;

		//relay
		static constexpr int PIN_RELAY_1 = 3;
		static constexpr int PIN_RELAY_2 = 10;
		static constexpr int PIN_RELAY_3 = 6;
		static constexpr int PIN_RELAY_4 = 7;
		static constexpr int PIN_RELAY_5 = 5;
		static constexpr int PIN_RELAY_6 = 4;
		static constexpr int PIN_RELAY_7 = 0;
		static constexpr int PIN_RELAY_8 = 1;
		static constexpr int PIN_RELAY_9 = 18;
		static constexpr int PIN_RELAY_10 = 19;

		static constexpr std::array<int, RELAY_PINS_COUNT> PINS_RELAY({
			PIN_RELAY_1,
			PIN_RELAY_2,
			PIN_RELAY_3,
			PIN_RELAY_4,
			PIN_RELAY_5,
			PIN_RELAY_6,
			PIN_RELAY_7,
			PIN_RELAY_8,
			PIN_RELAY_9,
			PIN_RELAY_10,
			});
	}
}