#include "fb_http_puller.hpp"

#include <cstring>

#include "fb_globals.hpp"
#include "fb_wifi.hpp"

#include "cJSON.h"



using namespace fb;
using namespace project;



HttpPuller::HttpPuller(ActionCb onResult)
	: _actionCb(std::move(onResult))
{

}

const char* HttpPuller::getName() const
{
	return "HttpPuller";
}

void HttpPuller::handleEvent(const event::Event& event)
{
	if(event.groupId != event::EventGroup::WIFI){
		return;
	}

	if(event.eventId == wifi::WifiEventId::CONNECTED){
		_onWifiConnected();
	}else if(event.eventId == wifi::WifiEventId::DISCONNECTED){
		_onWifiDisconnected();
	}
}

void HttpPuller::setUrl(std::string_view str)
{
	_url = str;
}

std::string_view HttpPuller::getUrl() const
{
	return _url;
}

void HttpPuller::start()
{
	if(_task){
		FB_DEBUG_LOG_W_OBJ("Already started!");
		return;
	}

	auto ret = xTaskCreate(&HttpPuller::_taskFunc, getName(), 4 * 1024, this, 15, &_task);
	assert(ret == pdPASS);

	FB_DEBUG_LOG_I_OBJ("Created task");
}

void HttpPuller::_onWifiConnected()
{
	//need mutex
	_deinitRequest();
	_initRequest();

	_isWifiConnected = true;
}

void HttpPuller::_onWifiDisconnected()
{
	_isWifiConnected = false;
}

bool HttpPuller::_onPerformRequest()
{
	esp_http_client_set_url(_httpClient, _url.c_str());
	// Выполнение HTTPS-запроса (блокирующий режим)
	esp_err_t err = esp_http_client_perform(_httpClient);

	if (err == ESP_OK) {
		// Получение статуса ответа
		//need mutex due to missing 
		int status_code = esp_http_client_get_status_code(_httpClient);
		if (status_code == 200) {
			_onSuccess(std::string_view(_responseBuff, _length));
			return true;

		} else if (status_code == 404) {
			_onFailure();
		} else {
			FB_DEBUG_LOG_E_OBJ("Unexpected status code: %d", status_code);
			_onFailure();
		}
	} else {
		_onFailure();
		//for safety close all connections and recreate client
		//need mutex for this
		_deinitRequest();
		_initRequest();
	}

	return false;
}

void HttpPuller::_onFailure()
{
	FB_DEBUG_LOG_I_OBJ("Failed to get something or do request");
	std::invoke(_actionCb, std::optional<std::string_view>{});
}

void HttpPuller::_onSuccess(std::string_view data)
{
	FB_DEBUG_LOG_I_OBJ("Data: %s", data.cbegin());
	std::invoke(_actionCb, std::optional<std::string_view>{data});
}

void HttpPuller::_initRequest()
{
	if(_httpClient){
		return;
	}

	FB_DEBUG_ENTER_I_OBJ();

	esp_http_client_config_t config;
	memset(&config, 0, sizeof(config));
	
	config.url = _url.c_str();
	config.cert_pem = NULL; // Не использовать сертификат для проверки
	config.skip_cert_common_name_check = true; // Пропустить проверку имени сертификата
	config.disable_auto_redirect = false;
	config.timeout_ms = 5000;
	config.method = HTTP_METHOD_GET;
	config.buffer_size = 512;
	config.buffer_size_tx = 512;
	config.event_handler = &HttpPuller::_requestHandler;
	config.user_data = this;
	config.keep_alive_enable = true;
	config.keep_alive_idle = 5;
	config.keep_alive_interval = 5;
	config.keep_alive_count = 3;

	// Инициализация HTTP-клиента
	_httpClient = esp_http_client_init(&config);
}

void HttpPuller::_deinitRequest()
{
	if(!_httpClient){
		return;
	}

	FB_DEBUG_ENTER_I_OBJ();

	// Завершение работы HTTP-клиента
	esp_http_client_cleanup(_httpClient);
	_httpClient = nullptr;
}

void HttpPuller::_taskFunc(void *arg)
{
	auto* self = static_cast<HttpPuller*>(arg);
	// const char* TAG = self->getName();

	for(;;){
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

esp_err_t HttpPuller::_requestHandler(esp_http_client_event_t *evt)
{
	auto* self = static_cast<HttpPuller*>(evt->user_data);

	if(evt->event_id == HTTP_EVENT_ON_HEADER){
		self->_length = 0;		

	}else if(evt->event_id == HTTP_EVENT_ON_DATA){
		memcpy(self->_responseBuff + self->_length, evt->data, evt->data_len);
		self->_length += evt->data_len;
		self->_responseBuff[self->_length] = '\0';
	}

	return ESP_OK;
}