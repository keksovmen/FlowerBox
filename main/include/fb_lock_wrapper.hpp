#pragma once



#include "freertos/FreeRTOS.h"
#include "freertos/semphr.h"



namespace fb
{
	namespace util
	{
		class LockWrapper
		{
		public:
			LockWrapper(SemaphoreHandle_t hndl, TickType_t tickToWait)
				: _hndl(hndl)
			{
				if (hndl)
				{
					_result = xSemaphoreTakeRecursive(hndl, tickToWait);
				}
			}

			~LockWrapper()
			{
				if (_hndl && _result == pdPASS)
				{
					xSemaphoreGiveRecursive(_hndl);
				}
			}

			operator bool()
			{
				return _result == pdPASS;
			}

		private:
			SemaphoreHandle_t _hndl;
			BaseType_t _result = pdFAIL;
		};
	}
}