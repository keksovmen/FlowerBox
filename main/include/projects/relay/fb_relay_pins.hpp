#pragma once



#include <array>



namespace fb
{
	namespace pins
	{

		//keyboard
		static constexpr int PIN_KEYBOARD_RESET = 9;

		//relay
		#if _HW_VERSION == 2
			static constexpr std::array RELAY_PINS = {2, 3, 10, 6, 7, 11, 5, 4, 8, 18, 0, 1, 12};
		#else
			static constexpr std::array RELAY_PINS = {3, 10, 6, 7, 5, 4, 0, 1, 18, 19};
		#endif
	}
}