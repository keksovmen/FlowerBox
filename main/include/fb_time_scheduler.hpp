#pragma once



#include <functional>
#include <queue>

#include "fb_debug.hpp"

#include "freertos/FreeRTOS.h"
#include "freertos/semphr.h"
#include "freertos/task.h"
#include "freertos/timers.h"



namespace fb
{
	namespace util
	{
		class TimeScheduler : public debug::Named
		{
			public:
				using Action = std::function<void()>;



				virtual const char* getName() const override;

				void start();

				bool addActionPrecise(Action action, int unixAt, int maxWaitMs);
				bool addActionDelayed(Action action, int afterMs, int maxWaitMs, bool repeated = false);
				bool addActionDelayedIrq(Action action, int afterMs, int maxWaitMs, bool repeated = false);

			
			private:
				struct ActionWrapper
				{
					Action action;
					int period;
					bool repeated;

					void operator()(){std::invoke(action);}
				};



				using Entry = std::pair<uint32_t, ActionWrapper>;
				struct _Comparator
				{
					bool operator()(const Entry& lhs, const Entry& rhs){return lhs.first > rhs.first;};
				};
				
				// static constexpr auto _compare = [](const Entry& lhs, const Entry& rhs){return lhs.first < rhs.first;};



				std::priority_queue<Entry, std::vector<Entry>, _Comparator> _queue;
				SemaphoreHandle_t _mutex = NULL;
				TaskHandle_t _taskHndl = NULL;
				TimerHandle_t _timerHndl = NULL;



				bool _isReadyToFire();
				void _prefromAction();
				void _startTimer();

				static void _task(void* me);
				static void _timer(TimerHandle_t timer);

		};
	}
}