#include "fb_http_server.hpp"

#include <cstdio>

#include "fb_debug.hpp"

#include "esp_http_server.h"



using namespace fb;
using namespace http;



static const char* TAG = "fb_http_server";



static httpd_handle_t _server = nullptr;



static esp_err_t _fileHandler(httpd_req_t *r)
{
	FB_DEBUG_TAG_ENTER();
	FB_DEBUG_TAG_LOG("File: %s", static_cast<const char*>(r->user_ctx));

	auto* f = std::fopen(static_cast<const char*>(r->user_ctx), "rb");
	if(!f){
		FB_DEBUG_TAG_LOG_E("Failed to open: %s", static_cast<const char*>(r->user_ctx));
		return httpd_resp_send_404(r);
	}

	esp_err_t err;
	char buffer[512];
	int count;

	while((count = std::fread(buffer, sizeof(*buffer), sizeof(buffer), f)) != 0){
		FB_DEBUG_TAG_LOG("Read %d bytes", count);
		if(std::ferror(f)){
			FB_DEBUG_TAG_LOG_W("File error occurred!");
			break;
		}

		err = httpd_resp_send_chunk(r, buffer, count);
		if(err != ESP_OK){
			FB_DEBUG_TAG_LOG_E("Http send chunk error occurred! %d", err);
			break;
		}

		if(std::feof(f)){
			FB_DEBUG_TAG_LOG("File EOF occurred");
			break;
		}
	}

	err = httpd_resp_send_chunk(r, buffer, 0);
	if(err != ESP_OK){
		FB_DEBUG_TAG_LOG_E("Http send chunk error occurred! %d", err);
	}

	std::fclose(f);

	FB_DEBUG_TAG_EXIT();

	return err;
}



static void _initServer()
{
	httpd_config_t cfg = HTTPD_DEFAULT_CONFIG();
	ESP_ERROR_CHECK(httpd_start(&_server, &cfg));
}

static void _registerUris()
{
	const httpd_uri_t index = {
		.uri = "/",
		.method = HTTP_GET,
		.handler = &_fileHandler,
		.user_ctx = reinterpret_cast<void*>(const_cast<char*>("/spiffs/index.html")),
	};

	httpd_register_uri_handler(_server, &index);
}


void http::startServer()
{
	FB_DEBUG_TAG_ENTER();

	if(_server){
		FB_DEBUG_TAG_LOG("Already initialized!");
		return;
	}

	_initServer();
	_registerUris();

	FB_DEBUG_TAG_EXIT();
}

void http::stopServer()
{
	FB_DEBUG_TAG_ENTER();

	httpd_stop(_server);
	_server = nullptr;

	FB_DEBUG_TAG_EXIT();
}
