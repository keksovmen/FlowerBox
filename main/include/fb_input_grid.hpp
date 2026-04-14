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
				using Rows = std::array<int, ROW>;
				using Columns = std::array<int, COLUMN>;
				using ActionCb = std::function<void(std::pair<int, int>)>;
				using MatrixArray = std::array<int, ROW * COLUMN>;



				InputGrid(int chargeMs, int dischargeMs, const Rows& rows, const Columns& columns)
					: _chargeMs(chargeMs), _dischargeMs(dischargeMs), _rows(rows), _columns(columns)
				{
				}

				void init(const ActionCb& cb)
				{
					_actionCb = cb;

					for(int in : _columns){
						gpio_set_direction(static_cast<gpio_num_t>(in), GPIO_MODE_INPUT);
					}

					for(int out : _rows){
						gpio_config_t cfg = {
							.pin_bit_mask = 1ull << out,
							.mode = GPIO_MODE_OUTPUT,
							.pull_up_en = GPIO_PULLUP_DISABLE,
							.pull_down_en = GPIO_PULLDOWN_DISABLE,
							.intr_type = GPIO_INTR_DISABLE,
						};
						gpio_config(&cfg);
					}
				}

				virtual const char* getName() const override
				{
					return "InputGrid";
				};

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

							const int index = y * COLUMN + x;

							if(_states[index] && !isPressed){
								FB_DEBUG_LOG_I_OBJ("Released button: [%d; %d]/ R:%d - C:%d", x, y, (int)out, (int)in);
								std::invoke(_actionCb, std::pair<int, int>{x, y});
							}

							_states[index] = isPressed;
						}
						
						//return to default state
						gpio_set_level(out, 1);

						//now wait for capacitors to charge up
						vTaskDelay(pdMS_TO_TICKS(_chargeMs));
					}
				}
			
			private:
				const int _chargeMs;
				const int _dischargeMs;
				const Rows _rows;
				const Columns _columns;
				ActionCb _actionCb;

				MatrixArray _states;



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