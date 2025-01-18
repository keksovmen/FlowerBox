#include "fb_server_debug.hpp"

#include <string_view>
#include <utility>

#include "fb_debug.hpp"

#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"
#include "freertos/semphr.h"
#include "freertos/task.h"

#include "esp_log.h"



#define _LOG_BUFFER_SIZE_KB 8
#define _TASK_STASK_SIZE 4 * 1024
#define _TASK_PRIORITY 15


using namespace fb;
using namespace server;



//[0] = begining of address, [1] = data length
using Entry = std::pair<int, int>;



//TODO: put in to template engine
static const char body_start[] = R"(
	<!DOCTYPE html>
	<html>
	<head>
	  <title>Log console</title>
	</head>
	<body>
	  <div id="content">)";

//TODO: put in to template engine, due to dynamic IP
static const char body_end[] = R"(
	</div>
	<script>
	// Create a WebSocket connection
	const socket = new WebSocket('ws://192.168.0.194:80/debug/live');

	// Handle the WebSocket connection
	socket.onopen = function() {
		console.log('WebSocket connection established.');
	};

	socket.onmessage = function(event) {
		// Get the HTML content from the WebSocket message
		const htmlContent = event.data;

		// Update the content div with the received HTML
		const contentDiv = document.getElementById('content');
		contentDiv.innerHTML += htmlContent;
	};

	socket.onclose = function() {
		console.log('WebSocket connection closed.');
	};

	socket.onerror = function(error) {
		console.error('WebSocket error:', error);
	};
	</script>
	</body>
	</html>)";

static const char* TAG = "fb_server_debug";



static std::array<char, _LOG_BUFFER_SIZE_KB * 1024> _buffer;
static int _writeIndex = 0;
static int _readIndex = 0;

static vprintf_like_t _original = NULL;
static SemaphoreHandle_t _mutex = NULL;

static httpd_handle_t _server_hndl = NULL;
static int _client_socket = 0;

static QueueHandle_t _entry_queue = NULL;



static int _logPrintf(const char* format, va_list args)
{
	char tmp[256];
	const int tmp_result = vsprintf(tmp, format, args);
	std::string_view view{tmp, static_cast<size_t>(tmp_result)};

	//without initial color, without reset color, and without \n symbol
	const int clear_size = tmp_result - sizeof(LOG_COLOR_I) - sizeof(LOG_RESET_COLOR);

	//transform into html
	char color[16];
	if(view.starts_with(LOG_COLOR_I)){
		strcpy(color, "green");
	}else if(view.starts_with(LOG_COLOR_W)){
		strcpy(color, "orange");
	}else if(view.starts_with(LOG_COLOR_W)){
		strcpy(color, "red");
	}else{
		strcpy(color, "black");
	}

	char html[256];
	const int result = sprintf(html, "<p style=\"color: %s;\">%.*s</p>", color, clear_size, tmp + sizeof(LOG_COLOR_I));

	//take mutex
	if(xSemaphoreTakeRecursive(_mutex, pdMS_TO_TICKS(1000)) != pdPASS){
		//original must work in any case
		return _original(format, args);
	};

	Entry entry{_writeIndex, result};

	//put data
	if(_buffer.size() - _writeIndex <= result){
		//write by two parts
		const int part = _buffer.size() - _writeIndex;
		//write first part than another
		strncpy(_buffer.data() + _writeIndex, html, part);
		//write remaining
		strcpy(_buffer.data(), html + part);

		_writeIndex = result - part;	
	}else{
		//write fully
		strcpy(_buffer.data() + _writeIndex, html);

		_writeIndex += result;
	}

	//send after write is done
	xQueueSend(_entry_queue, &entry, 0);

	//release mutex
	xSemaphoreGiveRecursive(_mutex);

	return _original(format, args);
}



static esp_err_t _debug_cb(httpd_req_t* r)
{
	//send head of HTML
	httpd_resp_send_chunk(r, body_start, sizeof(body_start) - 1);

	if(xSemaphoreTakeRecursive(_mutex, pdMS_TO_TICKS(1000)) != pdPASS){
		httpd_resp_send_500(r);

		return ESP_FAIL;
	};
	
	// send stored messages
	if(_readIndex > _writeIndex){
		//case when write looped over
		const int part = _buffer.size() - _readIndex;
		httpd_resp_send_chunk(r, _buffer.data() + _readIndex, part);
		httpd_resp_send_chunk(r, _buffer.data(), _writeIndex);
	}else{
		//case when read is behind the write or the same
		httpd_resp_send_chunk(r, _buffer.data() + _readIndex, _writeIndex - _readIndex);
	}
	//update indexes
	_readIndex = _writeIndex;

	xSemaphoreGiveRecursive(_mutex);

	//send rest of HTML
	httpd_resp_send_chunk(r, body_end, sizeof(body_end) - 1);
	//send finished
	httpd_resp_send_chunk(r, NULL, 0);


	return ESP_OK;
}

static esp_err_t _web_socket_cb(httpd_req_t* r)
{
	if (r->method == HTTP_GET)
	{
		FB_DEBUG_LOG_I_TAG("WebSocket established for fd = %d", httpd_req_to_sockfd(r));

		_server_hndl = r->handle;
		_client_socket = httpd_req_to_sockfd(r);
		
		return ESP_OK;
	}
	else if(r->method == HTTP_DELETE)
	{
		FB_DEBUG_LOG_I_TAG("WebSocket closed for fd = %d", httpd_req_to_sockfd(r));

		_client_socket = -1;

		return ESP_OK;
	}

	return ESP_OK;
}



void _task(void* arg)
{
	for(;;)
	{
		//listen for log entries
		Entry entry{};
		if(xQueueReceive(_entry_queue, &entry, portMAX_DELAY) != pdTRUE){
			continue;
		}

		//send them to all opened web sockets
		if(_client_socket == -1){
			continue;
		}

		httpd_ws_frame_t frame = {
			.final = true,
			.fragmented = false,
			.type = HTTPD_WS_TYPE_TEXT,
			.payload = reinterpret_cast<uint8_t*>(_buffer.data() + entry.first),
			.len = static_cast<size_t>(entry.second),
		};

		//determine will it fit in to 2 packets
		if(entry.first + entry.second >= _buffer.size()){
			//will have to send as two packets
			frame.final = false,
			frame.fragmented = true,
			frame.len = static_cast<size_t>(_buffer.size() - entry.first),
			httpd_ws_send_frame_async(_server_hndl, _client_socket, &frame);

			frame.final = true;
			frame.type = HTTPD_WS_TYPE_CONTINUE;
			frame.payload = reinterpret_cast<uint8_t*>(_buffer.data());
			frame.len = entry.second - (_buffer.size() - entry.first);
			httpd_ws_send_frame_async(_server_hndl, _client_socket, &frame);
		}
		else{
			//send as one packet
			httpd_ws_send_frame_async(_server_hndl, _client_socket, &frame);
		}
	}
}



void server::registerServerDebug(Builder& builder)
{
	_mutex = xSemaphoreCreateMutex();
	assert(_mutex != NULL);

	_original = esp_log_set_vprintf(&_logPrintf);
	assert(_original != NULL);

	_entry_queue = xQueueCreate(16, sizeof(Entry));
	assert(_entry_queue);

	assert(xTaskCreate(&_task, "server_debug", _TASK_STASK_SIZE, NULL, _TASK_PRIORITY, NULL) == pdPASS);

	builder.addEndpoint(Endpoint{"/debug", EndpointMethod::GET, nullptr, &_debug_cb});
	builder.addEndpoint(Endpoint{"/debug/live", EndpointMethod::WEB_SOCKET, nullptr, &_web_socket_cb});

}