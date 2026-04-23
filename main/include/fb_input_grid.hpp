#pragma once



#include <array>
#include <functional>
#include <span>
#include <utility>

#include "fb_debug.hpp"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "driver/gpio.h"



namespace fb
{
	namespace util
	{
		template<int ROW, int COLUMN>
		class InputGrid : public debug::Named
		{
			public:
				enum class ButtonAction : int
				{
					PRESSED,
					STILL_PRESSED,
					RELEASED,
				};

				struct ButtonEntry
				{
					int index;
					ButtonAction action;
					int durationMs;
				};



			public:
				using Rows = std::array<int, ROW>;
				using Columns = std::array<int, COLUMN>;
				using ActionCb = std::function<void(ButtonEntry)>;



				InputGrid(int chargeMs, int dischargeMs, const Rows& rows, const Columns& columns)
					: _chargeMs(chargeMs), _dischargeMs(dischargeMs), _rows(rows), _columns(columns)
				{
				}

				void init(const ActionCb& cb, int debounceMs, int stillPressedPeriodMs)
				{
					_actionCb = cb;
					_debounceMs = debounceMs;
					_stillPressedMs = stillPressedPeriodMs;

					gpio_config_t cfg = {
						.pin_bit_mask = 1ull,
						.mode = GPIO_MODE_OUTPUT,
						.pull_up_en = GPIO_PULLUP_DISABLE,
						.pull_down_en = GPIO_PULLDOWN_DISABLE,
						.intr_type = GPIO_INTR_DISABLE,
					};

					for(int in : _columns){
						cfg.pin_bit_mask = 1ull << in;
						cfg.mode = GPIO_MODE_INPUT;

						gpio_config(&cfg);
					}

					for(int out : _rows){
						cfg.pin_bit_mask = 1ull << out;
						cfg.mode = GPIO_MODE_OUTPUT;

						gpio_config(&cfg);
					}
				}

				virtual const char* getName() const override
				{
					return "InputGrid";
				}

				void start(int taskPriority, int taskDepth)
				{
					xTaskCreate(&_task, "MatrixReader", taskDepth, this, taskPriority, NULL);
				}

				void tick()
				{
					for(int y = 0; y < ROW; y++)
					{
						const gpio_num_t out = static_cast<gpio_num_t>(_rows[y]);
						//first drive low
						gpio_set_level(out, 0);
						//now wait for capacitance to discharge
						vTaskDelay(pdMS_TO_TICKS(_dischargeMs));

						for(int x = 0; x < COLUMN; x++)
						{
							const gpio_num_t in = static_cast<gpio_num_t>(_columns[x]);
							const bool isPressed = !gpio_get_level(in);

							const int index = x * ROW + y;

							_handleButton(index, isPressed);
						}
						
						//return to default state
						gpio_set_level(out, 1);

						//now wait for capacitors to charge up
						vTaskDelay(pdMS_TO_TICKS(_chargeMs));
					}
				}
			
			private:
				struct _ButtonInfo
				{
					uint32_t durationMs = 0;
					uint32_t cycles = 0;
					uint32_t previousTick = 0;
					ButtonAction action = ButtonAction::RELEASED;



					void press()
					{
						action = ButtonAction::PRESSED;
						cycles = 0;
						reset();
					}

					void release()
					{
						action = ButtonAction::RELEASED;
					}

					void reset()
					{
						durationMs = 0;
						previousTick = xTaskGetTickCount();
					}

					void incrementDuration()
					{
						const auto current = xTaskGetTickCount();
						durationMs += pdTICKS_TO_MS(current - previousTick);
						previousTick = current;
					}

					bool passDebounce(int debounceMs)
					{
						if(action == ButtonAction::STILL_PRESSED){
							return false;
						}

						incrementDuration();
						if(durationMs > debounceMs){
							action = ButtonAction::STILL_PRESSED;
							reset();
							return true;
						}

						return false;
					}

					bool updateHoldTime(int stillPressDurationMs)
					{
						incrementDuration();
						if(durationMs > stillPressDurationMs){
							cycles++;
							reset();
							return true;
						}

						return false;
					}
				};

				//TODO: update to be able to send STILL_PRESSED, PRESSED events, and apply bouncing
				using MatrixArray = std::array<_ButtonInfo, ROW * COLUMN>;
			
			private:
				const int _chargeMs;
				const int _dischargeMs;
				const Rows _rows;
				const Columns _columns;
				ActionCb _actionCb;

				int _debounceMs;
				int _stillPressedMs;

				MatrixArray _states;



				void _handleButton(int index, bool isPressed)
				{
					auto& s = _states[index];

					if((s.action == ButtonAction::RELEASED) && isPressed){
						s.press();

					}else if(s.action == ButtonAction::PRESSED && isPressed){
						if(s.passDebounce(_debounceMs)){
							std::invoke(_actionCb, ButtonEntry{index, ButtonAction::PRESSED, 0});
						}
					}else if(s.action == ButtonAction::STILL_PRESSED && isPressed){
						if(s.updateHoldTime(_stillPressedMs)){
							std::invoke(_actionCb, ButtonEntry{index, ButtonAction::STILL_PRESSED, (int) s.cycles * _stillPressedMs});
						}
					}else if(s.action == ButtonAction::STILL_PRESSED && !isPressed){
						std::invoke(_actionCb, ButtonEntry{index, ButtonAction::RELEASED, (int) s.cycles * _stillPressedMs + (int) s.durationMs});
						s.release();

					}else if(s.action == ButtonAction::PRESSED && !isPressed){
						s.release();
					}
				}



				static void _task(void* arg)
				{
					auto* me = static_cast<InputGrid*>(arg);
					for(;;){
						me->tick();
					}

					vTaskDelete(NULL);
				}

		};
	}
}