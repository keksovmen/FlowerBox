#pragma once



#include <functional>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "freertos/semphr.h"
#include "freertos/timers.h"

#include "fb_debug.hpp"



namespace fb
{
	namespace util
	{
		class ServiceIface : public debug::Named
		{
			public:
				using Action = std::function<void(ServiceIface*)>;



				virtual ~ServiceIface();

				void start();
				void stop();

				void addAction(const Action& action);
				void forcePullAction();

				void setTimerPeriod(int ms);
				int getTimerPeriod() const;

			private:
				QueueHandle_t _actionQueue;
				TimerHandle_t _timerHndl;



				//задача
				static void _task(void* arg);
				static void _timer(TimerHandle_t timer);
				static void _pullAction(ServiceIface* me);

				virtual void _onPull() = 0;
		};
	}
}