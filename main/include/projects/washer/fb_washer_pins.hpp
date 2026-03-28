#pragma once



namespace fb
{
	namespace pins
	{
		//start from top and go in right circle, dash is the last
		// static constexpr int SEGMENTS[] = {2, 3, 10, 6, 7, 11, 5};
		static constexpr int SEGMENTS[] = {2, 3, 10, 6, 7, 11, 1};
		// static constexpr int DIGITS[] = {4, 8, 9};
		static constexpr int DIGITS[] = {12, 18, 19, 13};
		static constexpr int BUZZER = 0;
		// static constexpr int KEYBOARD_INPUT = 1;
		static constexpr int KEYBOARD_INPUT = 5;
		static constexpr int PIN_WASHER_2_INPUT = 8;
		static constexpr int PIN_KEYBOARD_RESET = 9;
	}
}