#include "fb_http_puller_32.hpp"

#include <cstring>



using namespace fb;
using namespace project;



HttpPuller::HttpPuller(ActionCb onResult)
	: AbstractHttpPuller(onResult)
{

}

const char* HttpPuller::getName() const
{
	return "HttpPuller";
}

void HttpPuller::_initRequest()
{
	if(_httpClient){
		return;
	}

	FB_DEBUG_ENTER_I_OBJ();

	esp_http_client_config_t config;
	memset(&config, 0, sizeof(config));
	
	config.url = getUrl().begin();
	config.cert_pem = NULL; // Не использовать сертификат для проверки
	config.skip_cert_common_name_check = true; // Пропустить проверку имени сертификата
	config.disable_auto_redirect = false;
	config.timeout_ms = 5000;
	config.method = HTTP_METHOD_GET;
	config.buffer_size = 512;
	config.buffer_size_tx = 512;
	config.event_handler = &HttpPuller::_requestHandler;
	config.user_data = this;
	// config.keep_alive_enable = true;
	// config.keep_alive_idle = 5;
	// config.keep_alive_interval = 5;
	// config.keep_alive_count = 3;

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

bool HttpPuller::_onPerformRequest()
{
	esp_http_client_set_url(_httpClient, getUrl().begin());
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

esp_err_t HttpPuller::_requestHandler(esp_http_client_event_t *evt)
{
	auto* self = static_cast<HttpPuller*>(evt->user_data);

	if(evt->event_id == HTTP_EVENT_ON_HEADER){
		self->_length = 0;		

	}else if(evt->event_id == HTTP_EVENT_ON_DATA){
		if(self->_length + evt->data_len > sizeof(self->_responseBuff)){
			return ESP_FAIL;
		}
		memcpy(self->_responseBuff + self->_length, evt->data, evt->data_len);
		self->_length += evt->data_len;
		self->_responseBuff[self->_length] = '\0';
	}

	return ESP_OK;
}