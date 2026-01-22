#pragma once



#include <array>
#include <functional>

#include "fb_debug.hpp"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_heap_caps.h"



namespace fb
{
	namespace periph
	{
		class IrReader : public debug::Named
		{
			public:
				/**
				 * @brief 
				 * @param val 0 bit is first accepted bit = the oldest, the higher bits are the newest
				 */
				using DataCb = std::function<void(int pin, uint32_t val, int length)>;



				virtual const char* getName() const override;

				void init();
				void addPin(int pin);
				void startTask(int priority, int stackSize, DataCb cb);

				

			private:
				template<int SIZE>
				class Entry
				{
					public:
						const int N = SIZE;
						int pin = -1;



						int64_t operator[](int i) const
						{
							return _stampsUs1[i];
						}

						void push(int64_t stamp)
						{
							_stampsUs1[_length % SIZE] = stamp;
							_length++;
						}

						void reset()
						{
							_length = 0;
							memset(_stampsUs1, 0, sizeof(_stampsUs1));
						}

						void setMessageFlag()
						{
							_messageFlag = true;
						}

						void clearMessageFlag()
						{
							_messageFlag = false;
						}

						int64_t lastStamp(int64_t def) const
						{
							if(_length <= 0){
								return def;
							}

							return _stampsUs1[_length - 1];
						}

						bool hasMessage() const
						{
							return _messageFlag;
						}

						int currentLength() const
						{
							return _length;
						}

					private:
						int _length = 0;	//current bit
						int64_t _stampsUs1[SIZE];
						bool _messageFlag = false;
				};

			private:
				DataCb _dataCb;
				std::array<Entry<64>, 10> _pins;
				int _length = 0;
				TaskHandle_t _taskHndl = nullptr;



				static void IRAM_ATTR _irq(void* arg);
				static void IRAM_ATTR _task(void *pvParameters);
		};
	}
}