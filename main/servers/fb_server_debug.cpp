#include "fb_server_debug.hpp"

#include <string_view>
#include <utility>

#include "fb_buffer.hpp"
#include "fb_debug.hpp"
#include "fb_lock_wrapper.hpp"

#include "freertos/FreeRTOS.h"
#include "freertos/semphr.h"
#include "freertos/task.h"



#define _LOG_BUFFER_SIZE_KB 16
#define _TASK_STASK_SIZE 4 * 1024
#define _TASK_PRIORITY 15
#define _INVALID_SOCKET -1



using namespace fb;
using namespace server;



//TODO: put in to template engine
static const char body_start[] = R"(
	<!DOCTYPE html>
	<html>
	<head>
		<title>Log console</title>
		<style type="text/css">
			#holder {
				width: 800px;
				margin: auto;
			}

			#content {
				height: 600px;
				overflow: scroll;
				margin: auto;
			}

			p {
				margin: 0px;
			}
		</style>
	</head>
	<body>
	<div id="holder">
		<button id="button" onclick=toggleScroll()>Enable scroll</button>
		<div id="content">)";

//TODO: put in to template engine, due to dynamic IP
static const char body_end[] = R"(
		</div>
	</div>
	<script>
		var scrollEnable = false;
	
	const socket = new WebSocket(`ws://${location.host}/debug/live`);

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

		if(scrollEnable){
			contentDiv.scroll(0, contentDiv.scrollHeight);
		}
	};

	socket.onclose = function() {
		console.log('WebSocket connection closed.');
	};

	socket.onerror = function(error) {
		console.error('WebSocket error:', error);
	};

	function toggleScroll() {
		scrollEnable = !scrollEnable;
		const button = document.getElementById('button');
		button.innerText = scrollEnable ? "Disable scroll" : "Enable scroll";
	}
	</script>
	</body>
	</html>)";

static const char* TAG = "fb_server_debug";



static util::Buffer<_LOG_BUFFER_SIZE_KB * 1024> _buffer;

static vprintf_like_t _original = NULL;
static SemaphoreHandle_t _buffer_mutex = NULL;

static httpd_handle_t _server_hndl = NULL;
static volatile int _client_socket = _INVALID_SOCKET;

static TaskHandle_t _task_hndl = NULL;



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
	}else if(view.starts_with(LOG_COLOR_E)){
		strcpy(color, "red");
	}else{
		strcpy(color, "black");
	}

	char html[256];
	const int result = sprintf(html, "<p style=\"color: %s;\">%.*s</p>", color, clear_size, tmp + sizeof(LOG_COLOR_I));

	{
		//take lock
		util::LockWrapper lock(_buffer_mutex, pdMS_TO_TICKS(1000));

		if(!lock){
			//original must work in any case
			return _original(format, args);
		};

		_buffer.putData(std::string_view(html, result));
	}

	//send after write is done
	if(_task_hndl){
		xTaskNotifyGive(_task_hndl);
	}


	return _original(format, args);
}



static esp_err_t _debug_cb(httpd_req_t* r)
{
	//send head of HTML
	httpd_resp_send_chunk(r, body_start, sizeof(body_start) - 1);

	{
		util::LockWrapper lock(_buffer_mutex, pdMS_TO_TICKS(1000));

		if(!lock){
			httpd_resp_send_500(r);

			return ESP_FAIL;
		};
		
		// send stored messages
		if(_buffer.isPartitioned()){
			//case when write looped over
			auto data = _buffer.readPartition();
			auto begining = data.find("<p");


			if(!data.empty() && begining != std::string_view::npos){
				FB_DEBUG_LOG_E_TAG("Write partition, len %d", data.length() - begining);
				httpd_resp_send_chunk(r, data.data() + begining, data.length() - begining);
			}
		}

		auto data = _buffer.readPartition();
		auto begining = data.find("<p");

		if(!data.empty() && begining != std::string_view::npos){
			FB_DEBUG_LOG_E_TAG("Write, len %d", data.length() - begining);
			httpd_resp_send_chunk(r, data.data() + begining, data.length() - begining);
		}
	}

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

		_client_socket = _INVALID_SOCKET;

		return ESP_OK;
	}

	return ESP_OK;
}



void _task(void* arg)
{
	for(;;)
	{
		//listen for log entries
		ulTaskNotifyTake(pdFALSE, portMAX_DELAY);

		//send them to all opened web sockets
		if(_client_socket == _INVALID_SOCKET){
			continue;
		}

		if(httpd_ws_get_fd_info(_server_hndl, _client_socket) != HTTPD_WS_CLIENT_WEBSOCKET){
			continue;
		}

		std::string_view data;
		bool two_pass = false;

		//to read data from buffer
		{
			util::LockWrapper lock(_buffer_mutex, portMAX_DELAY);

			two_pass = _buffer.isPartitioned();
			data = _buffer.readPartition();
		}

		if(data.empty()){
			continue;
		}

		httpd_ws_frame_t frame = {
			.final = true,
			.fragmented = false,
			.type = HTTPD_WS_TYPE_TEXT,
			.payload = (uint8_t*) data.data(),
			.len = static_cast<size_t>(data.length()),
		};

		//determine will it fit in to 2 packets
		if(two_pass){
			//will have to send as two packets
			//send first
			frame.final = false,
			frame.fragmented = true,
			httpd_ws_send_data(_server_hndl, _client_socket, &frame);

			//second packet
			{
				util::LockWrapper lock(_buffer_mutex, portMAX_DELAY);
				data = _buffer.readPartition();
			}

			//configure second
			frame.final = true;
			frame.type = HTTPD_WS_TYPE_CONTINUE;
			frame.payload = (uint8_t*) data.data(),
			frame.len = static_cast<size_t>(data.length());
		}

		//send as one packet
		httpd_ws_send_data(_server_hndl, _client_socket, &frame);
	}
}



void server::initServerDebug()
{
	_buffer_mutex = xSemaphoreCreateMutex();
	assert(_buffer_mutex != NULL);

	_original = esp_log_set_vprintf(&_logPrintf);
	assert(_original != NULL);
}

void server::registerServerDebug(Builder& builder)
{
	assert(xTaskCreate(&_task, "server_debug", _TASK_STASK_SIZE, NULL, _TASK_PRIORITY, &_task_hndl) == pdPASS);

	builder.addEndpoint(Endpoint{"/debug", EndpointMethod::GET, nullptr, &_debug_cb});
	builder.addEndpoint(Endpoint{"/debug/live", EndpointMethod::WEB_SOCKET, nullptr, &_web_socket_cb});
}