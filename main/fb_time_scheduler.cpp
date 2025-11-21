#include "fb_time_scheduler.hpp"

#include "fb_lock_wrapper.hpp"



using namespace fb;
using namespace util;



const char* TimeScheduler::getName() const
{
	return "TimeScheduler";
}

void TimeScheduler::start()
{
	_mutex = xSemaphoreCreateRecursiveMutex();
	assert(_mutex);

	_timerHndl = xTimerCreate(getName(), portMAX_DELAY, pdFALSE, this, &_timer);
	assert(_timerHndl);

	auto ret = xTaskCreate(&_task, getName(), 4 * 1024, this, 15, &_taskHndl);
	assert(ret == pdPASS);
}

bool TimeScheduler::addActionPrecise(Action action, int unixAt, int maxWaitMs)
{
	return false;
}

bool TimeScheduler::addActionDelayed(Action action, int afterMs, int maxWaitMs)
{
	auto lock = LockWrapper(_mutex, pdMS_TO_TICKS(maxWaitMs));
	if(!lock){
		return false;
	}

	auto fireAt = pdTICKS_TO_MS(xTaskGetTickCount()) + afterMs;

	FB_DEBUG_LOG_I_OBJ("Adding action after %d ms, will fire at %u, current size %d", afterMs, fireAt, _queue.size());

	_queue.emplace(fireAt, action);
	_startTimer();
	
	return true;
}

bool TimeScheduler::_isReadyToFire()
{
	if(_queue.empty()){
		return false;
	}

	auto lock = LockWrapper(_mutex, pdMS_TO_TICKS(1000));
	if(!lock){
		return false;
	}

	return _queue.top().first < pdTICKS_TO_MS(xTaskGetTickCount());
}

void TimeScheduler::_prefromAction()
{
	FB_DEBUG_ENTER_I_OBJ();

	Action action;

	{
		auto lock = LockWrapper(_mutex, portMAX_DELAY);
		action = _queue.top().second;
		_queue.pop();
	}

	std::invoke(action);
}

void TimeScheduler::_startTimer()
{
	if(_queue.empty()){
		return;
	}

	auto lock = LockWrapper(_mutex, portMAX_DELAY);
	int64_t delta = static_cast<int64_t>(_queue.top().first) - static_cast<int64_t>(pdTICKS_TO_MS(xTaskGetTickCount()));

	FB_DEBUG_LOG_I_OBJ("Starting timer for %lld ms", delta);

	auto ret = xTimerStop(_timerHndl, pdMS_TO_TICKS(1000));
	assert(ret == pdPASS);

	ret = xTimerChangePeriod(_timerHndl, delta <= 0 ? 1 : pdMS_TO_TICKS(delta), pdMS_TO_TICKS(1000));
	assert(ret == pdPASS);

	ret = xTimerStart(_timerHndl, pdMS_TO_TICKS(1000));
	assert(ret == pdPASS);
}

void TimeScheduler::_task(void* me)
{
	TimeScheduler* mePtr = reinterpret_cast<TimeScheduler*>(me);

	for(;;)
	{
		ulTaskNotifyTake(pdTRUE, portMAX_DELAY);

		while(mePtr->_isReadyToFire()){
			mePtr->_prefromAction();
		}

		mePtr->_startTimer();
	}
}

void TimeScheduler::_timer(TimerHandle_t timer)
{
	TimeScheduler* mePtr = reinterpret_cast<TimeScheduler*>(pvTimerGetTimerID(timer));

	xTaskNotifyGive(mePtr->_taskHndl);
}
