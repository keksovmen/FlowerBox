#pragma once



namespace fb
{
	namespace pins
	{
		static constexpr int PIN_KEYBOARD_RESET = 9;
		static constexpr int PINS_RECEIVER[] = {2, 3, 6, 7, 10};
		static constexpr int PINS_LED[] = {0, 4, 5, 18, 19};
		static constexpr int RECEIVER_COUNT = sizeof(PINS_RECEIVER) / sizeof(PINS_RECEIVER[0]);
		static constexpr int PIN_TRANSCEIVER = 1;
	}
}