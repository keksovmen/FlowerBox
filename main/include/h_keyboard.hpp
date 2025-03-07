#pragma once



#include <functional>
#include <vector>

#include "h_buttons.hpp"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"



namespace h
{
	/**
	 * @brief объект кнопки на клавиатуре
	 * 
	 * Отслеживает текущее состояние и его переключение
	 */

	class KeyboardButton
	{
		public:
			const gpio_num_t pin;
			const ButtonVK vk;


			KeyboardButton(gpio_num_t pin, ButtonVK vk) : pin(pin), vk(vk){}

			/**
			 * @brief вернет предыдущее состояние
			 */

			bool press();

			/**
			 * @brief вернет предыдущее состояние
			 */
			
			bool release();

			/**
			 * @brief вернет время нахождения в нажатом состоянии
			 */
			uint32_t holdDurationMs();

			/**
			 * @return uint32_t вернет время нахождения в нажатом состоянии с момента _currentStart
			 */

			uint32_t currentDurationMs();

			/**
			 * @brief установит время отсчета для периода нажатия _currentStart в 0 
			 */

			void resetCurrentDuration();

		private:
			bool _isPressed = false;
			TickType_t _whenPressed;
			TickType_t _currentStart;
	};



	/**
	 * @brief клавиатура, считывает состояния кнопок и генерирует с них события,
	 * что отправляются в Consumer
	 * 
	 * Создает для себя поток
	 */

	class Keyboard
	{
		public:
			using Consumer = std::function<void(const ButtonAction&)>;



			static Keyboard* instance();


		private:
			Keyboard(){}	//private constructor
		public:


			/**
			 * @param consumer колбэк куда будет кидать нажатия кнопок
			 */

			void setConsumer(const Consumer& consumer){_consumer = consumer;}

			/**
			 * @param periodMs период опроса пинов
			 */

			void setPullPeriod(int periodMs){_pullPeriodMs = periodMs;}

			/**
			 * @param durationMs минимальная продолжительность нажатия, для избегания дребезга
			 */

			void setMinimumPressDuration(int durationMs){_minPressDurationMs = durationMs;}

			/**
			 * @brief определяет когда кнопка нажата и все еще не отпущена,
			 * через какие промежутки генерировать событие STILL_PRESSED
			 * 
			 * @param periodMs 
			 */

			void setRepeatPeriod(int periodMs){_repeatPeriodMs = periodMs;}

			/**
			 * @brief добавляет кнопку для обработки
			 * 
			 * @param pin не надо настраивать, сам настроит
			 * @param vk какой виртуальный код сгенерирует
			 */

			void keyboardAddButton(gpio_num_t pin, ButtonVK vk);

			/**
			 * @brief запускает поток обработки кнопок
			 * 
			 * @param stackSize размер стэка
			 * @param priority приоритет
			 * @param core ядро, -1 любое >= 0 заданное
			 * @return true запустил поток или он уже работает
			 * @return false не хватило памяти или чего-то еще
			 */

			bool keyboardStart(int stackSize, int priority, int core);

			/**
			 * @brief останавливает и в конце удаляет поток обработки кнопок
			 * Не блокирует поток, удалит его не сразу
			 */

			void keyboardStop();
		
		private:
			static Keyboard* _instance;



			Consumer _consumer;

			int _pullPeriodMs = 25;
			int _minPressDurationMs = 50;
			int _repeatPeriodMs = 300;
			
			std::vector<KeyboardButton> _buttons;

			volatile bool _isTaskRunning = false;



			static void _task(void* arg);
	};
}