#include "fb_http_server.hpp"

#include "fb_debug.hpp"

#include "esp_http_server.h"



using namespace fb;
using namespace http;



static const char* TAG = "fb_http_server";



static httpd_handle_t _server = nullptr;



static esp_err_t _indexHandler(httpd_req_t *r)
{
	FB_DEBUG_TAG_ENTER();

	const esp_err_t err = httpd_resp_send(r, "<html><head></head><body><h1>Hello world</h1></body>", HTTPD_RESP_USE_STRLEN);

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
	httpd_uri_t index = {
		.uri = "/",
		.method = HTTP_GET,
		.handler = &_indexHandler,
		.user_ctx = nullptr,
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
