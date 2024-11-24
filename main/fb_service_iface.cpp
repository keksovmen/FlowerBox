#include "fb_service_iface.hpp"



#define _QUEUE_SIZE 5

#define _TASK_STACK_SIZE 4 * 1024
#define _TASK_PRIORITY 10

#define _DEFAULT_PULL_PERIOD_MS 3000



using namespace fb;
using namespace util;



void ServiceIface::start()
{
	_actionQueue = xQueueCreate(_QUEUE_SIZE, sizeof(ServiceIface::Action));
	assert(_actionQueue);

	_timerHndl = xTimerCreate(getName(), pdMS_TO_TICKS(_DEFAULT_PULL_PERIOD_MS), pdTRUE, this, &ServiceIface::_timer);
	assert(_timerHndl);

	auto err = xTimerStart(_timerHndl, portMAX_DELAY);
	assert(err = pdPASS);

	err = xTaskCreate(&ServiceIface::_task, getName(), _TASK_STACK_SIZE, this, _TASK_PRIORITY, NULL);
	assert(err == pdPASS);

	forcePullAction();
}

void ServiceIface::stop()
{

}

void ServiceIface::addAction(const Action& action)
{
	auto err = xQueueSend(_actionQueue, &action, portMAX_DELAY);
	assert(err == pdPASS);
}

void ServiceIface::forcePullAction()
{
	auto action = std::function(&ServiceIface::_pullAction);
	addAction(action);
}

void ServiceIface::setTimerPeriod(int ms)
{
	assert(_timerHndl);
	
	xTimerChangePeriod(_timerHndl, pdMS_TO_TICKS(ms), portMAX_DELAY);
}

int ServiceIface::getTimerPeriod() const
{
	assert(_timerHndl);
	
	return pdTICKS_TO_MS(xTimerGetPeriod(_timerHndl));
}

void ServiceIface::_task(void* arg)
{
	ServiceIface *me = reinterpret_cast<ServiceIface*>(arg);

	for (;;)
	{
		ServiceIface::Action action;

		auto err = xQueueReceive(me->_actionQueue, &action, portMAX_DELAY);
		assert(err == pdPASS);

		std::invoke(action, me);
	}

	vTaskDelete(NULL);
}

void ServiceIface::_timer(TimerHandle_t timer)
{
	ServiceIface* me = reinterpret_cast<ServiceIface*>(pvTimerGetTimerID(timer));
	me->forcePullAction();
}

void ServiceIface::_pullAction(ServiceIface* me)
{
	me->_onPull();
}

