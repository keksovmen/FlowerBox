#pragma once



namespace fb
{
	namespace pins
	{
		static constexpr int TX_PIN = 18;
		static constexpr int RX_PINS[] = {2, 3, 10, 6, 7, 11, 5, 4, 12, 13};
		// static constexpr int RX_PINS[] = {2, 3, 10, 6, 7};
		static constexpr int RX_PINS_SIZE = sizeof(RX_PINS) / sizeof(RX_PINS[0]);

		static constexpr int PIN_KEYBOARD_RESET = 9;
	}
}