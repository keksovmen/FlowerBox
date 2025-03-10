#pragma once



#include <stdint.h>



namespace h
{
	/**
	 * @brief виртуальные кнопки
	 */

	enum class ButtonVK
	{
		VK_0 = 0,
		VK_1,
		VK_2,
		VK_3,
		VK_4,
		VK_5,
		VK_6,
		VK_7,
		VK_8,
	};



	/**
	 * @brief текущие кнопки
	 */

	enum class ButtonKeys
	{
		UP = 0,	//up
		DOWN,	//down
		CHANNEL,	//channel switch
		PLAY_STOP,	//play stop
		SELECT,	//ok
		BACK,	//esc
		MODE,	//mode
		RESET,
	};



	/**
	 * @brief "движение" кнопок
	 */

	enum class ButtonMovement
	{
		PRESSED,		//когда нажимаешь кнопку и до этого не была нажата
		STILL_PRESSED,	//когда жмешь кнопку и она до этого была нажата
		RELEASED		//когда отжимаешь кнопку
	};



	struct ButtonAction
	{
		ButtonKeys button;
		uint32_t holdMs;			//на момент генерации события
		ButtonMovement movement;


		bool isJustPressed(ButtonKeys key) const {
			return movement == ButtonMovement::RELEASED &&
				key == button;
		}

		bool isLongJustPressed(ButtonKeys key, uint32_t durationMs) const {
			return movement == ButtonMovement::RELEASED &&
				holdMs >= durationMs &&
				key == button;
		}

		bool isPressedForMs(ButtonKeys key, uint32_t durationMs) const {
			return holdMs >= durationMs &&
				key == button;
		}

		bool isStillPressedForMs(ButtonKeys key, uint32_t durationMs) const {
			return movement == ButtonMovement::STILL_PRESSED &&
				holdMs >= durationMs &&
				key == button;
		}

		bool isStillPressed(ButtonKeys key) const {
			return isStillPressedForMs(key, 0);
		}

		bool isReleased() const {
			return movement == ButtonMovement::RELEASED;
		}

		bool isSelectPressed() const {
			return isJustPressed(ButtonKeys::SELECT) || isJustPressed(ButtonKeys::PLAY_STOP);
		}

		bool isExitPressed() const {
			return isJustPressed(ButtonKeys::BACK);
		}
	};


	
	/**
	 * @brief преобразует виртуальную кнопку в текущую
	 * 
	 * @param vk 
	 * @return ButtonKeys 
	 */

	inline ButtonKeys mapButtonVKtoButtonKey(ButtonVK vk)
	{
		switch (vk)
		{
		case ButtonVK::VK_0:
			return ButtonKeys::RESET;
		
		default:
			return ButtonKeys::RESET;
		}
	}
}