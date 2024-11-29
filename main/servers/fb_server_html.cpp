#include "fb_server_html.hpp"

#include <cstdio>
#include <cstring>
#include <string>
#include <unordered_map>

#include "fb_debug.hpp"
#include "fb_update.hpp"



#define _PATH_PREFIX "/spiffs/"
#define _HTML_PATH "/html/"


using namespace fb;
using namespace server;



static const char* TAG = "fb_server_html";



static std::unordered_map<std::string, HtmlFileCb> _fileMap;



static std::string _composeFileName(std::string_view base, std::string_view uri)
{
	//must skip all preceding / in uri
	// auto tmp = std::string(uri);
	const auto beginning = uri.find_first_not_of('/');

	return std::string(base) + std::string(uri.substr(beginning == std::string::npos ? 0 : beginning));
}

static std::string_view _removePathPreffix(std::string_view uri)
{
	assert(uri.starts_with(_HTML_PATH));

	return uri.substr(strlen(_HTML_PATH));
}

static esp_err_t _staticHandler(httpd_req_t *r, const char* fileName)
{
	FB_DEBUG_TAG_ENTER();

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

	return err;
}

static esp_err_t _templateHandler(httpd_req_t *r, const char* fileName, HtmlFileCb& cb)
{
	FB_DEBUG_TAG_ENTER();

	char buffer[512];

	templates::Engine engine(buffer, sizeof(buffer), 
		[r](const char* data, int size){
			httpd_resp_send_chunk(r, data, size);
		});

	std::invoke(cb, engine);
	
	if(engine.process(fileName)){
		return httpd_resp_send_chunk(r, buffer, 0);
	}else{
		return httpd_resp_send_500(r);
	}
}

static esp_err_t _fileCb(httpd_req_t *r)
{
	FB_DEBUG_TAG_ENTER();
	const std::string file = _composeFileName(static_cast<const char*>(r->user_ctx), _removePathPreffix(r->uri));
	const char* fileName = file.c_str();
	FB_DEBUG_TAG_LOG("uri: %s\n\tfile name: %s", r->uri, fileName);

	esp_err_t err = ESP_OK;
	if(_fileMap.contains(std::string(_removePathPreffix(r->uri)))){
		err = _templateHandler(r, fileName, _fileMap.at(std::string(_removePathPreffix(r->uri))));
	}else{
		err = _staticHandler(r, fileName);
	}

	FB_DEBUG_TAG_EXIT();

	return err;
}



void server::registerServerHtml(Builder& builder)
{
    builder.addEndpoint(Endpoint{_HTML_PATH "*", EndpointMethod::GET, reinterpret_cast<void*>(const_cast<char*>(_PATH_PREFIX)), &_fileCb});
}

void server::htmlAddFileHandler(std::string_view fileName, HtmlFileCb fileCb)
{
	_fileMap[std::string(fileName)] = fileCb;
}