#pragma once



#include <array>
#include <functional>
#include <random>

#include "fb_debug.hpp"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"



namespace fb
{
	namespace util
	{
		template<int N>
		class AcDimmer : public debug::Named
		{
			public:
				enum class Mode
				{
					OFF = 0,
					ON,
					DIMMED,
					BLINK,
					MAX
				};



				using SetGpioCb = std::function<void(int index, bool state)>;



				AcDimmer(SetGpioCb cb) : _setCb(cb)
				{

				}

				virtual const char* getName() const
				{
					return "AcDimmer";
				}

				void setState(int index, Mode mode)
				{
					if(index < 0 || index >= N){
						FB_DEBUG_LOG_E_OBJ("Illegal index: %d", index);
						return;
					}

					FB_DEBUG_LOG_I_OBJ("Set %d as mode %d", index, (int) mode);

					_modes[index] = mode;
					_progress[index].tick = 0;
				}

				void setBlinkPeriod(int index, int minTicks, int maxTicks)
				{
					if(index < 0 || index >= N){
						FB_DEBUG_LOG_E_OBJ("Illegal index: %d", index);
						return;
					}

					FB_DEBUG_LOG_I_OBJ("Set %d new period %d-%d", index, minTicks, maxTicks);

					_progress[index].minTick = minTicks;
					_progress[index].maxTick = maxTicks;
				}

				void startTask(int priority, int stackSize)
				{
					xTaskCreate(&_task, "Ac_Dimmer", stackSize, this, priority, nullptr);
				}

				void tick()
				{
					for(int i = 0; i < N; i++){
						Mode m = _modes[i];

						switch(m)
						{
							case Mode::OFF:
								_handleOff(i);
								break;

							case Mode::ON:
								_handleOn(i);
								break;

							case Mode::DIMMED:
								_handleDimmed(i);
								break;

							case Mode::BLINK:
								_handleBlink(i);
								break;
							
							default: break;
						}
					}
				}

				Mode getMode(int index) const
				{
					if(index < 0 || index >= N){
						FB_DEBUG_LOG_E_OBJ("Illegal get index: %d", index);
						return Mode::MAX;
					}

					return _modes[index];
				}


			private:
				struct Entry
				{
					int tick = 0;
					int periodTicks = 0;
					bool state = false;
					int minTick = 1;
					int maxTick = 100;
				};

			private:
				SetGpioCb _setCb;

				std::array<Mode, N> _modes;
				std::array<Entry, N> _progress;
				std::mt19937 _generator{std::random_device{}()};
				//TODO: make max and min as dynamic arguments
				std::uniform_int_distribution<int> _distribution{1, 100};



				void _handleOff(int index)
				{
					if(_progress[index].tick == 0){
						std::invoke(_setCb, index, false);
						_progress[index].tick++;
					}
				}

				void _handleOn(int index)
				{
					if(_progress[index].tick == 0){
						std::invoke(_setCb, index, true);
						_progress[index].tick++;
					}
				}

				void _handleDimmed(int index)
				{
					std::invoke(_setCb, index, _progress[index].tick % 2);
					_progress[index].tick++;
				}

				void _handleBlink(int index)
				{
					auto& entry = _progress[index];
					if(entry.tick == entry.periodTicks){
						//reset tick counter
						entry.tick = 0;
						entry.periodTicks = _distribution(_generator, typename decltype(_distribution)::param_type(entry.minTick, entry.maxTick));
						//swap current state
						entry.state = !entry.state;
						std::invoke(_setCb, index, entry.state);
					}else{
						entry.tick++;
					}
				}

				static void _task(void* arg)
				{
					AcDimmer* me = static_cast<AcDimmer*>(arg);
					
					for(;;)
					{
						me->tick();
						vTaskDelay(pdMS_TO_TICKS(10));
					}
				}

		};
	}
}