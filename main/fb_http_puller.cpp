#include "fb_http_puller.hpp"

#include <cstring>

#include "fb_globals.hpp"
#include "fb_wifi.hpp"

#include "cJSON.h"



using namespace fb;
using namespace project;



AbstractHttpPuller::AbstractHttpPuller(ActionCb onResult)
	: _actionCb(std::move(onResult))
{

}

void AbstractHttpPuller::handleEvent(const event::Event& event)
{
	if(event.groupId != event::EventGroup::WIFI){
		return;
	}

	if(event.eventId == wifi::WifiEventId::CONNECTED){
		_onWifiConnected();
	}else if(event.eventId == wifi::WifiEventId::FAILED_TO_CONNECT){
		_onWifiDisconnected();
	}
}

void AbstractHttpPuller::start()
{
	if(_task){
		FB_DEBUG_LOG_W_OBJ("Already started!");
		return;
	}

	auto ret = xTaskCreate(&AbstractHttpPuller::_taskFunc, getName(), 4 * 1024, this, 15, &_task);
	assert(ret == pdPASS);

	FB_DEBUG_LOG_I_OBJ("Created task");
}

void AbstractHttpPuller::setUrl(std::string_view str)
{
	_url = str;
}

std::string_view AbstractHttpPuller::getUrl() const
{
	return _url;
}


void AbstractHttpPuller::setPause(bool state)
{
	_pause = state;
}

void AbstractHttpPuller::setTimeoutMs(int ms)
{
	_timeoutMs = ms;
}


bool AbstractHttpPuller::isWorking() const
{
	return !_pause;
}

int AbstractHttpPuller::getTimeoutMs() const
{
	return _timeoutMs;
}

void AbstractHttpPuller::_onFailure()
{
	FB_DEBUG_LOG_I_OBJ("Failed to get something or do request");
	std::invoke(_actionCb, std::optional<std::string_view>{});
}

void AbstractHttpPuller::_onSuccess(std::string_view data)
{
	FB_DEBUG_LOG_I_OBJ("Data: %s", data.cbegin());
	std::invoke(_actionCb, std::optional<std::string_view>{data});
	vTaskDelay(pdMS_TO_TICKS(_timeoutMs));
}

void AbstractHttpPuller::_taskFunc(void *arg)
{
	auto* self = static_cast<AbstractHttpPuller*>(arg);
	// const char* TAG = self->getName();

	for(;;){
		if(self->_pause){
			vTaskDelay(pdMS_TO_TICKS(100));
			continue;
		}
		//wait for wifi connection
		if(!self->_isWifiConnected){
			vTaskDelay(pdMS_TO_TICKS(1000));
			continue;
		}

		if(!self->_onPerformRequest()){
			vTaskDelay(pdMS_TO_TICKS(500));
		}
	}

}

void AbstractHttpPuller::_onWifiConnected()
{
	//need mutex
	_deinitRequest();
	_initRequest();

	_isWifiConnected = true;
}

void AbstractHttpPuller::_onWifiDisconnected()
{
	_isWifiConnected = false;
}