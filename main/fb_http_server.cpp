/**
 * @file fb_http_server.cpp
 * @author your name (you@domain.com)
 * @brief URI rules must be considered in .html files and any others
 * 		each actual page MUST end with file extension -> we now it is this file we want to fetch
 * 		THERE IS NO FOLDERS so /path/file.ext is invalid must be only file.ext -> in html specify absolute paths
 * @version 0.1
 * @date 2024-01-13
 * 
 * @copyright Copyright (c) 2024
 * 
 */



#include "fb_http_server.hpp"

#include <cstdio>
#include <cstring>
#include <string>

#include "fb_debug.hpp"

#include "esp_http_server.h"



#define _PATH_PREFIX "/spiffs/"
#define _FILE_INDEX "index.html"
#define _FILE_STYLE "style.css"
#define _FILE_UPDATE "update.html"

#define _HEADER_CONTENT_TYPE "Content-Type"
#define _CONTENT_TYPE_BOUNDARY "boundary="
#define _DATA_DELIMETER "\r\n\r\n"



using namespace fb;
using namespace http;

using DataCb = void(*)(const char* data, int size);



static const char* TAG = "fb_http_server";



static httpd_handle_t _server = nullptr;



static bool _composeSeparator(httpd_req_t* r, char* separator, int size)
{
	char tmp[256];
	const esp_err_t err = httpd_req_get_hdr_value_str(r, _HEADER_CONTENT_TYPE, tmp, sizeof(tmp) - 1);
	if(err != ESP_OK){
		//todo add 400 error, missing content type
		return false;
	}

	const char* at = strstr(tmp, _CONTENT_TYPE_BOUNDARY);
	sscanf(at + strlen(_CONTENT_TYPE_BOUNDARY), "%s", tmp);
	
	sprintf(separator, "\r\n--%.*s--", size - 7, tmp);

	return true;
}



static esp_err_t _multipartFileInputHandler(httpd_req_t* r, DataCb cb)
{
	FB_DEBUG_TAG_ENTER();

	FB_DEBUG_TAG_LOG("Content length = %d", r->content_len);

	char separator[128];

	if(!_composeSeparator(r, separator, sizeof(separator))){
		httpd_resp_send_err(r, HTTPD_400_BAD_REQUEST, "Not found boundary at: Content-Type: boundary=xxxxxx");
		return ESP_FAIL;
	}

	const int separator_len = strlen(separator);
	FB_DEBUG_TAG_LOG("Found separator: %s, length: %d", separator, separator_len);

	char buffer[512];
	//0 - looking for data beginning
	//1 - providing data
	//2 - looking for data end
	int state = 0;
	int actual_data = 0;
	int remaining = r->content_len;

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
				httpd_resp_send_500(r);

	            return ESP_FAIL;
			}
		}

		//decrease counter
		remaining -= received;

		switch (state)
		{
		case 0:
		{
			char* at = strstr(out, _DATA_DELIMETER);
			assert(at);
			out = at + strlen(_DATA_DELIMETER);

			FB_DEBUG_TAG_LOG("Found data at offset: %u", at - buffer);
			
			/* fall through */
			state = 1;
			__attribute__((fallthrough));
		}
		
		case 1:
		{
			int valid_len = 0;
			if(remaining == 0){
				//separator in current block
				char* at = strstr(out, separator);
				valid_len = at - out;

			}else if(remaining <= separator_len){
				//separator in next block, this block contains last drop of data
				const int separator_len_in_block = separator_len - remaining;
				valid_len = received - (((int)(out - buffer)) - separator_len_in_block);
				//change state to next
				state = 2;

			}else if(remaining > separator_len){
				//separator in next blocks
				valid_len = received - (out - buffer);
			}

			actual_data += valid_len;
			FB_DEBUG_TAG_LOG("Received [%d / %d], actual data [%d/%d]", received, remaining, valid_len, actual_data);

			break;
		}
		
		case 2:
			{
				FB_DEBUG_TAG_LOG("Received [%d / %d], no actual data in this block", received, remaining);
				break;

			}
		
		default:
			assert(0);
			break;
		}
	}

	FB_DEBUG_TAG_EXIT();

	return ESP_OK;
}



static std::string _composeFileName(const char* base, const char* uri)
{
	//must skip all preceding / in uri
	auto tmp = std::string(uri);
	const auto beginning = tmp.find_first_not_of('/');

	return std::string(base) + tmp.substr(beginning == std::string::npos ? 0 : beginning);
}

static esp_err_t _fileHandler(httpd_req_t *r)
{
	FB_DEBUG_TAG_ENTER();
	const std::string file = _composeFileName(static_cast<const char*>(r->user_ctx), r->uri);
	const char* fileName = file.c_str();
	FB_DEBUG_TAG_LOG("uri: %s\nfile name: %s", r->uri, fileName);

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

	esp_err_t err = _multipartFileInputHandler(r, [](const char* data, int size){
		FB_DEBUG_TAG_LOG("Data block of size %d:\n%.*s", size, size, data);
	});

	err |= httpd_resp_sendstr(r, "<h1>Success</h1>");

	FB_DEBUG_TAG_EXIT();

	return err;
}



static void _initServer()
{
	httpd_config_t cfg = HTTPD_DEFAULT_CONFIG();
	cfg.max_resp_headers = 32;
	//turning on /* syntax for uris
	cfg.uri_match_fn = httpd_uri_match_wildcard;

	ESP_ERROR_CHECK(httpd_start(&_server, &cfg));
}

static void _registerUris()
{
	//TODO: use request->uri to define what file you need in file handler, so you can make uri as =/* everything, and remuve user_ctx
	const httpd_uri_t files = {
		.uri = "/*",
		.method = HTTP_GET,
		.handler = &_fileHandler,
		.user_ctx = reinterpret_cast<void*>(const_cast<char*>(_PATH_PREFIX)),
	};

	const httpd_uri_t update_post = {
		.uri = "/" _FILE_UPDATE,
		.method = HTTP_POST,
		.handler = &_updateHandler,
		.user_ctx = NULL,
	};

	httpd_register_uri_handler(_server, &files);
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
