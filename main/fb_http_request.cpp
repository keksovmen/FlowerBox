#include "fb_http_request.hpp"



using namespace fb;
using namespace periph;



const char* HttpRequest::getName() const
{
	return "HttpRequest";
}

void HttpRequest::init()
{
	if(_httpClient){
		return;
	}

	FB_DEBUG_ENTER_I_OBJ();

	esp_http_client_config_t config;
	memset(&config, 0, sizeof(config));
	
	config.url = "http://192.168.0.111:8080/loh";
	config.cert_pem = NULL; // Не использовать сертификат для проверки
	config.skip_cert_common_name_check = true; // Пропустить проверку имени сертификата
	config.disable_auto_redirect = false;
	config.timeout_ms = 5000;
	config.buffer_size = 512;
	config.buffer_size_tx = 512;
	config.event_handler = [](esp_http_client_event_t *evt){return ESP_OK;};
	config.user_data = this;

	// Инициализация HTTP-клиента
	_httpClient = esp_http_client_init(&config);
	// assert(_httpClient != nullptr);
}

void HttpRequest::post(std::string url, std::string data)
{
	esp_http_client_set_method(_httpClient, HTTP_METHOD_POST);
	esp_http_client_set_url(_httpClient, url.c_str());
	esp_http_client_set_post_field(_httpClient, data.c_str(), data.length());

    esp_err_t err = esp_http_client_perform(_httpClient);  // one-shot request[[ESP32 HTTP client](https://docs.espressif.com/projects/esp-idf/en/latest/esp32/api-reference/protocols/esp_http_client.html)]
    if (err == ESP_OK) {
        int status = esp_http_client_get_status_code(_httpClient);
        int content_length = esp_http_client_get_content_length(_httpClient);
        FB_DEBUG_LOG_I_OBJ("HTTP POST Status = %d, content_length = %d", status, content_length);
    } else {
        FB_DEBUG_LOG_E_OBJ("HTTP POST request failed: %s", esp_err_to_name(err));
    }

    // esp_http_client_cleanup(_httpClient);
}
