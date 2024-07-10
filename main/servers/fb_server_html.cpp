#include "fb_server_html.hpp"

#include <cstdio>
#include <cstring>
#include <string>

#include "fb_debug.hpp"
#include "fb_update.hpp"



#define _PATH_PREFIX "/spiffs/"
#define _FILE_INDEX "index.html"
#define _FILE_STYLE "style.css"
#define _FILE_UPDATE "update.html"



using namespace fb;
using namespace server;



static const char* TAG = "fb_server_html";



static std::string _composeFileName(const char* base, const char* uri)
{
	//must skip all preceding / in uri
	auto tmp = std::string(uri);
	const auto beginning = tmp.find_first_not_of('/');

	return std::string(base) + tmp.substr(beginning == std::string::npos ? 0 : beginning);
}

static esp_err_t _fileCb(httpd_req_t *r)
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



void server::registerServerHtml(Builder& builder)
{
    //TODO: use request->uri to define what file you need in file handler, so you can make uri as =/* everything, and remuve user_ctx
    builder.addEndpoint(Endpoint{"/*", EndpointMethod::GET, reinterpret_cast<void*>(const_cast<char*>(_PATH_PREFIX)), &_fileCb});
}