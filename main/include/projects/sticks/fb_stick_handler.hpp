#pragma once



#include <array>

#include "fb_debug.hpp"
#include "fb_wrappers.hpp"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"



namespace fb
{
	namespace project
	{
		template<int N>
		class StickHandler : public debug::Named
		{
			private:
				struct Stick
				{
					//it takes 15sec to start and finish game, and additional 60sec before going to sleep
					//make it a bit smaller due to edge case, better pulse once than twice
					static constexpr int SHUTDOWN_TIME_MS = 73*1000;
					//during this time one single pulse will pause the game and not restart it
					// static constexpr int GAME_TIME_MS = 13*1000;



					bool enabled = false;
					TickType_t activatedAt = 0;



					void updateClock()
					{
						const char* TAG = "DEBUG";

						if(!enabled){
							return;
						}

						auto current = xTaskGetTickCount();
						//probably we turned off already
						FB_DEBUG_LOG_W_TAG("Activated at: %u, current %u, delta %u, in MS %u", activatedAt, current, current - activatedAt, pdTICKS_TO_MS(current - activatedAt));
						if(pdTICKS_TO_MS(current - activatedAt) >= SHUTDOWN_TIME_MS){
							enabled = false;
						}
					}

					void resetClock()
					{
						activatedAt = xTaskGetTickCount();
					}

					bool isDoublePulse() const
					{
						//turned off case
						return !enabled;
					}

					void enable()
					{
						enabled = true;
						resetClock();
					}

					uint32_t deltaTimeMs() const
					{
						return pdTICKS_TO_MS(xTaskGetTickCount() - activatedAt);
					}
				};



			public:
				StickHandler(wrappers::WrapperIface& gpio) : _gpio(gpio)
				{

				}

				virtual const char* getName() const override
				{
					return "StickHandler";
				}

				void applyStates(const std::array<bool, N> states)
				{
					//determine which to pulse once which to pulse twice and update clocks
					int pulseOnce = 0;
					int pulseTwice = 0;
					
					for(auto i = 0; i < states.size(); i++){
						//skip if do nothing
						if(!states[i]){
							continue;
						}

						//transform to bit value for port
						int pinVal = 0;
						if(i == 1){
							//remap pin 1 (not working) to pin 4
							pinVal = 1 << 4;
						}else if(i >= 4){
							//remap starting from pin 4 to pin 4 + 1
							pinVal = 1 << (i + 1);
						}else{
							pinVal = 1 << i;
						}

						pulseOnce |= pinVal;

						//update clock and check if device is off then second pulse will be needed
						_sticks[i].updateClock();
						if(_sticks[i].isDoublePulse()){
							pulseTwice |= pinVal;
						}

						//apply reset of timer and state to enable
						_sticks[i].enable();
					}

					if(pulseOnce){
						FB_DEBUG_LOG_I_OBJ("Pulse once: 0x%X, twice: 0x%X", pulseOnce, pulseTwice);
						_pulse(pulseOnce);
						_pulse(pulseTwice);
					}
				}
			
			private:
				wrappers::WrapperIface& _gpio;
				std::array<Stick, N> _sticks;

				

				void _pulse(int portValue)
				{
					_gpio.setValue(portValue);
					vTaskDelay(pdMS_TO_TICKS(100));
					_gpio.setValue(0);
					vTaskDelay(pdMS_TO_TICKS(100));
				}
		};
	}
}