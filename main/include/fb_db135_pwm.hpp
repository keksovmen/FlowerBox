#pragma once



#include "fb_wrapper_db135.hpp"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"



namespace fb
{
	namespace util
	{
		template<int ChipsCount>
		class Db135_Pwm
		{
			public:
				enum class Mode : uint8_t
				{
					OFF = 0,
					ON = 1,
					BLINK = 2
				};



				Db135_Pwm(wrappers::WrapperDb135<ChipsCount>& db) : _db135(db)
				{

				}

				void startTask(int pulseTime, int priority)
				{
					_pulseTime = pulseTime;
					xTaskCreate(&_dimmerTask, "Db135_PWM", 4 * 1024, this, priority, NULL);
				}

				void setMode(int index, Mode mode)
				{
					if(index < 0) return;
					if(index >= (ChipsCount * 16)) return;

					_outputState[index] = mode;
				}

				void setMode(std::span<Mode, ChipsCount * 16> modes)
				{
					std::copy(modes.begin(), modes.end(), _outputState.begin());
				}

			
			private:
				wrappers::WrapperDb135<ChipsCount>& _db135;

				int _pulseTime = 1;
				std::array<Mode, ChipsCount * 16> _outputState;



				static void IRAM_ATTR _dimmerTask(void* data)
				{
					Db135_Pwm* me = static_cast<Db135_Pwm*>(data);

					const uint8_t RESOLUTION = 12;
					const uint8_t DUTY_COUNTER = RESOLUTION * me->_pulseTime;

					uint8_t currentDuty = 0;
					uint8_t cycle = 0;

					bool dutyDirection = true;

					for(;;){
						const bool dutyVal = (cycle % RESOLUTION) < currentDuty;

						std::array<uint16_t, ChipsCount> states;
						for(int chip = 0; chip < ChipsCount; chip++){
							int state = 0;
							for(uint8_t i = 0; i < 16; i++){
								const int position = chip * 16 + i;

								if(me->_outputState[position] == Mode::ON){
									state |= 1 << i;
					
								}else if(me->_outputState[position] == Mode::BLINK){
									state |= dutyVal << i;
								}
							}
							states[chip] = state;
						}

						me->_db135.setValue(states);


						//time to change duty
						if(cycle == DUTY_COUNTER){
							if(dutyDirection){
								currentDuty++;
								if(currentDuty == RESOLUTION){
									dutyDirection = false;
								}
							}else{
								currentDuty--;
								if(currentDuty == 0){
									dutyDirection = true;
								}
							}

							cycle = 0;

						}else{
							cycle++;
						}

						vTaskDelay(pdMS_TO_TICKS(1));
					}

					vTaskDelete(NULL);
				}

		};
	}
}