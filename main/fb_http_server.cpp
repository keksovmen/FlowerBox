#include "fb_http_server.hpp"

#include <cstdio>
#include <cstring>

#include "fb_debug.hpp"

#include "esp_http_server.h"



#define _PATH_PREFIX "/spiffs/"
#define _FILE_INDEX "index.html"
#define _FILE_STYLE "style.css"
#define _FILE_UPDATE "update.html"



using namespace fb;
using namespace http;



static const char* TAG = "fb_http_server";



static httpd_handle_t _server = nullptr;



static esp_err_t _fileHandler(httpd_req_t *r)
{
	FB_DEBUG_TAG_ENTER();
	const char* fileName = static_cast<const char*>(r->user_ctx);

	FB_DEBUG_TAG_LOG("File: %s", fileName);

	auto* f = std::fopen(fileName, "rb");
	if(!f){
		FB_DEBUG_TAG_LOG_E("Failed to open: %s", fileName);
		return httpd_resp_send_404(r);
	}

	if(std::strstr(fileName, ".css")){
		httpd_resp_set_type(r, "text/css");
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

static esp_err_t _updateHandler(httpd_req_t *r)
{
	FB_DEBUG_TAG_ENTER();

	FB_DEBUG_TAG_LOG("Content length = %d", r->content_len);

	//TODO: clean up

	char separator[128];
	int separator_len = 0;

	{
		char tmp[256];
		const esp_err_t err = httpd_req_get_hdr_value_str(r, "Content-Type", tmp, sizeof(tmp) - 1);
		if(err != ESP_OK){
			//todo add 400 error, missing content type
			return ESP_FAIL;
		}

		const char* beginning = strstr(tmp, "boundary=");
		sscanf(beginning + strlen("boundary="), "%s", tmp);
	
		sprintf(separator, "\r\n--%.*s--", sizeof(separator) - 7, tmp);

		separator_len = strlen(separator);
		FB_DEBUG_TAG_LOG("Found separator: %s, length: %d", separator, separator_len);
	}

	char buffer[512];
	bool data_found = false;
	bool data_end = false;
	int remaining = r->content_len;
	int actual_data = 0;



	//TODO: handle multipart data
	//first string is the delimeter of parts, opening do not ends with --
	//so we look for data section that is separated by \r\n from begining and end
	//------WebKitFormBoundaryBMZzMyhZeAIZM0b5
	//Content-Disposition: form-data; name="file"; filename="storage.bin"
	//Content-Type: application/octet-stream
	//
	//data
	//
	//------WebKitFormBoundaryBMZzMyhZeAIZM0b5--
	while (remaining > 0) {
		char* out = buffer;

		const int received = httpd_req_recv(r, out, remaining > sizeof(buffer) - 1 ? sizeof(buffer) -1 : remaining);
		buffer[received] = '\0';

		//error cases
		if (received <= 0) {
			if (received == HTTPD_SOCK_ERR_TIMEOUT) {
				/* Retry if timeout occurred */
				continue;
			}else{
				//handle unexpected client lost
				FB_DEBUG_TAG_LOG_W("Unexpected error in receiving data");
	            return ESP_FAIL;
			}
		}

		//find beginning of data
		if(!data_found){
			//find double delimiters
			char* at = strstr(out, "\r\n\r\n");
			assert(at);
			FB_DEBUG_TAG_LOG("Found data at %u", at - buffer);
			data_found = true;
			out = at + 4;
		}
		
		remaining -= received;

		if(!data_end){
			int valid_len = 0;
			if(remaining == 0){
				//separator in current block
				char* at = strstr(out, separator);
				valid_len = at - out;

			}else if(remaining <= separator_len){
				//separator in next block, this block contains last drop of data
				const int separator_len_in_block = separator_len - remaining;
				valid_len = received - (((int)(out - buffer)) - separator_len_in_block);
				data_end = true;

			}else if(remaining > separator_len){
				//separator in next blocks
				valid_len = received - (out - buffer);
			}

			actual_data += valid_len;

			FB_DEBUG_TAG_LOG("Received %d, remains %d, actual data %d:\n%.*s", received, remaining, actual_data, valid_len, out);
		}
	}

	const esp_err_t err = httpd_resp_send(r, NULL, 0);

	FB_DEBUG_TAG_EXIT();

	return err;
}



static void _initServer()
{
	httpd_config_t cfg = HTTPD_DEFAULT_CONFIG();
	cfg.max_resp_headers = 32;

	ESP_ERROR_CHECK(httpd_start(&_server, &cfg));
}

static void _registerUris()
{
	//TODO: use request->uri to define what file you need in file handler, so you can make uri as =/* everything, and remuve user_ctx
	const httpd_uri_t index = {
		.uri = "/",
		.method = HTTP_GET,
		.handler = &_fileHandler,
		.user_ctx = reinterpret_cast<void*>(const_cast<char*>(_PATH_PREFIX _FILE_INDEX)),
	};

	const httpd_uri_t style = {
		.uri = "/" _FILE_STYLE,
		.method = HTTP_GET,
		.handler = &_fileHandler,
		.user_ctx = reinterpret_cast<void*>(const_cast<char*>(_PATH_PREFIX _FILE_STYLE)),
	};

	const httpd_uri_t update = {
		.uri = "/" _FILE_UPDATE,
		.method = HTTP_GET,
		.handler = &_fileHandler,
		.user_ctx = reinterpret_cast<void*>(const_cast<char*>(_PATH_PREFIX _FILE_UPDATE)),
	};

	const httpd_uri_t update_post = {
		.uri = "/" _FILE_UPDATE,
		.method = HTTP_POST,
		.handler = &_updateHandler,
		.user_ctx = NULL,
	};

	httpd_register_uri_handler(_server, &index);
	httpd_register_uri_handler(_server, &style);
	httpd_register_uri_handler(_server, &update);
	httpd_register_uri_handler(_server, &update_post);
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
