#include "fb_server_debug.hpp"

#include "freertos/FreeRTOS.h"
#include "freertos/semphr.h"

#include "esp_log.h"



using namespace fb;
using namespace server;



static std::array<char, 10*1024> _buffer;
static int _writeIndex = 0;
static int _readIndex = 0;

static vprintf_like_t _original = NULL;
static SemaphoreHandle_t _mutex = NULL;



static int _logPrintf(const char* format, va_list args)
{
	char tmp[256];
	const int result = vsprintf(tmp, format, args);

	//take mutex
	if(xSemaphoreTakeRecursive(_mutex, pdMS_TO_TICKS(1000)) != pdPASS){
		return 0;
	};

	//put data
	if(_buffer.size() - _writeIndex <= result){
		const int part = _buffer.size() - _writeIndex;
		//write first part than another
		strncpy(_buffer.data() + _writeIndex, tmp, part);
		//write remainings
		strcpy(_buffer.data(), tmp + part);

		_writeIndex = result - part;	
	}else{
		//write full
		strcpy(_buffer.data() + _writeIndex, tmp);

		_writeIndex += result;
	}
	//release mutex

	xSemaphoreGiveRecursive(_mutex);

	return _original(format, args);
}



static esp_err_t _debug_cb(httpd_req_t* r)
{
	if(xSemaphoreTakeRecursive(_mutex, pdMS_TO_TICKS(1000)) != pdPASS){
		return ESP_FAIL;
	};

	if(_readIndex > _writeIndex){
		//case when write looped over
		const int part = _buffer.size() - _readIndex;
		httpd_resp_send_chunk(r, _buffer.data() + _readIndex, part);
		httpd_resp_send_chunk(r, _buffer.data(), _writeIndex);
	}else{
		//case when read is behind the write or the same
		httpd_resp_send_chunk(r, _buffer.data() + _readIndex, _writeIndex - _readIndex);
	}

	httpd_resp_send_chunk(r, NULL, 0);

	_readIndex = _writeIndex;

	xSemaphoreGiveRecursive(_mutex);

	return ESP_OK;
}



void server::registerServerDebug(Builder& builder)
{
	_mutex = xSemaphoreCreateMutex();
	assert(_mutex != NULL);

	_original = esp_log_set_vprintf(&_logPrintf);
	assert(_original != NULL);

	builder.addEndpoint(Endpoint{"/debug", EndpointMethod::GET, nullptr, &_debug_cb});

}