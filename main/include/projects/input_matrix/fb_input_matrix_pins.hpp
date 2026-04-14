#pragma once



#include <array>



namespace fb
{
	namespace pins
	{
		static constexpr int PIN_SC = 12;
		static constexpr int PIN_SCL = 1;
		static constexpr int PIN_MOSI = 0;
		static constexpr int PIN_KEYBOARD_RESET = 9;
		static constexpr std::array<int, 6> PIN_ROW = {2, 3, 10, 6, 7, 11};
		static constexpr std::array<int, 5> PIN_COLUMN = {9, 8, 4, 5, 13};
	}
}