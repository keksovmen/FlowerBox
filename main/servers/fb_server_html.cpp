#include "fb_server_html.hpp"

#include <cstdio>
#include <cstring>
#include <string>
#include <unordered_map>

#include "fb_debug.hpp"
#include "fb_update.hpp"
#include "fb_box.hpp"
#include "fb_globals.hpp"



#define _PATH_PREFIX "/spiffs/"
#define _HTML_PATH "/html/"

#define _TEMPLATE_KEYWORD_ARRAY_SIZE "count"
#define _TEMPLATE_KEYWORD_ARRAY_NAME "name"
#define _TEMPLATE_KEYWORD_ARRAY_DESCRIPTION "description"
#define _TEMPLATE_KEYWORD_ARRAY_ID "id"
#define _TEMPLATE_KEYWORD_ARRAY_VALUE "value"
#define _TEMPLATE_KEYWORD_ARRAY_LABEL "label"
#define _TEMPLATE_KEYWORD_ARRAY_TID "tid"

#define _URL_PROPERTY_SOURCE_BOX "device"
#define _URL_PROPERTY_SOURCE_SENSOR "sensor"
#define _URL_PROPERTY_SOURCE_SWITCH "switch"

#define _URL_QUERY_SOURCE "source"
#define _URL_QUERY_ID "id"



using namespace fb;
using namespace server;



static const char* TAG = "fb_server_html";



static std::unordered_map<std::string, HtmlFileCb> _fileMap;


static std::unordered_map<std::string, std::string> _map_query(std::string_view query)
{
	std::unordered_map<std::string, std::string> result;
	if(query.empty()){
		return result;
	}

	auto current = query;

	while(true){
		auto equals = current.find("=");
		if(equals == std::string_view::npos){
			//no more params
			break;
		}
	
		std::string_view key(current.begin(), equals);
		std::string_view value(key.end() + 1);
	
		auto nextParam = value.find("&");
		if(nextParam != std::string_view::npos){
			value = std::string_view(key.end() + 1, nextParam);
		}
	
		result[std::string(key)] = value;
		current = value.end() + 1;
	}

	return result;
}

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

static std::string_view _removePathQuery(std::string_view uri)
{
	auto end = uri.find("?");
	if(end == std::string_view::npos){
		return uri;
	}

	return uri.substr(0, end);
}

static esp_err_t _staticHandler(httpd_req_t *r, const char* fileName)
{
	FB_DEBUG_ENTER_I_TAG();

	auto* f = std::fopen(fileName, "rb");
	if(!f){
		FB_DEBUG_LOG_E_TAG("Failed to open: %s", fileName);
		return httpd_resp_send_404(r);
	}

	if(std::strstr(fileName, ".css")){
		httpd_resp_set_type(r, "text/css");
	}

	esp_err_t err;
	char buffer[512];
	int count;

	while((count = std::fread(buffer, sizeof(*buffer), sizeof(buffer), f)) != 0){
		FB_DEBUG_LOG_I_TAG("Read %d bytes", count);
		if(std::ferror(f)){
			FB_DEBUG_LOG_W_TAG("File error occurred!");
			break;
		}

		err = httpd_resp_send_chunk(r, buffer, count);
		if(err != ESP_OK){
			FB_DEBUG_LOG_E_TAG("Http send chunk error occurred! %d", err);
			break;
		}

		if(std::feof(f)){
			FB_DEBUG_LOG_I_TAG("File EOF occurred");
			break;
		}
	}

	err = httpd_resp_send_chunk(r, buffer, 0);
	if(err != ESP_OK){
		FB_DEBUG_LOG_E_TAG("Http send chunk error occurred! %d", err);
	}

	std::fclose(f);

	return err;
}

static esp_err_t _templateHandler(httpd_req_t *r, const char* fileName, HtmlFileCb& cb)
{
	FB_DEBUG_ENTER_I_TAG();

	char buffer[512];

	templates::Engine engine(buffer, sizeof(buffer), 
		[r](const char* data, int size){
			httpd_resp_send_chunk(r, data, size);
		});
	
	char query[128];
	httpd_req_get_url_query_str(r, query, sizeof(query));

	std::invoke(cb, engine, _map_query(query));
	
	if(engine.process(fileName)){
		return httpd_resp_send_chunk(r, buffer, 0);
	}else{
		return httpd_resp_send_500(r);
	}
}

static esp_err_t _fileCb(httpd_req_t *r)
{
	FB_DEBUG_ENTER_I_TAG();
	
	const std::string file = _composeFileName(static_cast<const char*>(r->user_ctx), _removePathQuery(_removePathPreffix(r->uri)));
	const char* fileName = file.c_str();
	FB_DEBUG_LOG_I_TAG("uri: %s\n\tfile name: %s", r->uri, fileName);

	esp_err_t err = ESP_OK;
	if(_fileMap.contains(std::string(_removePathQuery(_removePathPreffix(r->uri))))){
		err = _templateHandler(r, fileName, _fileMap.at(std::string(_removePathQuery(_removePathPreffix(r->uri)))));
	}else{
		err = _staticHandler(r, fileName);
	}

	return err;
}

static void _menu_cb(templates::Engine& engine, const std::unordered_map<std::string, std::string>& query)
{
	engine.addArgStr(global::getFlowerBox()->getName(), _TEMPLATE_KEYWORD_ARRAY_LABEL);
}

static void _sensors_cb(templates::Engine& engine, const std::unordered_map<std::string, std::string>& query)
{
	const auto& entries = global::getFlowerBox()->getSensors();
	engine.addIntArg(entries.size(), _TEMPLATE_KEYWORD_ARRAY_SIZE);

	for(const auto e : entries){
		engine.appendArgArray(e->getName(), _TEMPLATE_KEYWORD_ARRAY_NAME);
		engine.appendArgArray(e->getDescription(), _TEMPLATE_KEYWORD_ARRAY_DESCRIPTION);
		engine.appendArgArray(std::to_string(e->getId()), _TEMPLATE_KEYWORD_ARRAY_ID);
	}
}

static void _switches_cb(templates::Engine& engine, const std::unordered_map<std::string, std::string>& query)
{
	const auto& entries = global::getFlowerBox()->getSwitches();
	engine.addIntArg(entries.size(), _TEMPLATE_KEYWORD_ARRAY_SIZE);

	for(const auto e : entries){
		engine.appendArgArray(e->getName(), _TEMPLATE_KEYWORD_ARRAY_NAME);
		engine.appendArgArray(e->getDescription(), _TEMPLATE_KEYWORD_ARRAY_DESCRIPTION);
		engine.appendArgArray(std::to_string(e->getId()), _TEMPLATE_KEYWORD_ARRAY_ID);
		engine.appendArgArray(std::to_string(e->currentState()), _TEMPLATE_KEYWORD_ARRAY_VALUE);
	}
}

static void _properties_cb(templates::Engine& engine, const std::unordered_map<std::string, std::string>& query)
{
	//query type property and id property
	auto source = query.contains(_URL_QUERY_SOURCE) ? query.at(_URL_QUERY_SOURCE) : "";
	FB_DEBUG_LOG_I_TAG("Property template callback: source = %s", source.c_str());

	if(source.empty()){
		FB_DEBUG_LOG_E_TAG("Argument source is not specified!");
		return;
	}

	if(!query.contains(_URL_QUERY_ID)){
		FB_DEBUG_LOG_E_TAG("Argument ID is not specified!");
		return;
	}

	int id = std::atoi(query.at(_URL_QUERY_ID).c_str());

	FB_DEBUG_LOG_I_TAG("Property template callback: source = %s, id = %d", source.c_str(), id);

	const box::ObjectIface::PropertyArray* entriesIds = nullptr;

	if(source == _URL_PROPERTY_SOURCE_BOX){
		engine.addArgStr(global::getFlowerBox()->getName(), _TEMPLATE_KEYWORD_ARRAY_LABEL);
		entriesIds = &global::getFlowerBox()->getBoxsProperties();

	}else if(source == _URL_PROPERTY_SOURCE_SENSOR){
		engine.addArgStr(global::getFlowerBox()->getSensor(id)->getName(), _TEMPLATE_KEYWORD_ARRAY_LABEL);
		entriesIds = &global::getFlowerBox()->getSensor(id)->getPropertyDependencies();

	}else if(source == _URL_PROPERTY_SOURCE_SWITCH){
		engine.addArgStr(global::getFlowerBox()->getSwitch(id)->getName(), _TEMPLATE_KEYWORD_ARRAY_LABEL);
		entriesIds = &global::getFlowerBox()->getSwitch(id)->getPropertyDependencies();

	}else{
		FB_DEBUG_LOG_E_TAG("Error, uknown properties source %s", source.c_str());
		return;
	}

	engine.addIntArg(entriesIds->size(), _TEMPLATE_KEYWORD_ARRAY_SIZE);
	std::for_each(entriesIds->begin(), entriesIds->end(),
		[&engine](int i){
			auto e = global::getFlowerBox()->getProperty(i);
			engine.appendArgArray(e->getName(), _TEMPLATE_KEYWORD_ARRAY_NAME);
			engine.appendArgArray(e->getDescription(), _TEMPLATE_KEYWORD_ARRAY_DESCRIPTION);
			engine.appendArgArray(std::to_string(e->getId()), _TEMPLATE_KEYWORD_ARRAY_ID);
			engine.appendArgArray(e->getValue(), _TEMPLATE_KEYWORD_ARRAY_VALUE);
			engine.appendArgArray(std::to_string(std::to_underlying(e->getTid())), _TEMPLATE_KEYWORD_ARRAY_TID);
		});
}



void server::registerServerHtml(Builder& builder)
{
    builder.addEndpoint(Endpoint{_HTML_PATH "*", EndpointMethod::GET, reinterpret_cast<void*>(const_cast<char*>(_PATH_PREFIX)), &_fileCb});
	htmlAddFileHandler("main_menu.html", &_menu_cb);
	htmlAddFileHandler("sensors.html", &_sensors_cb);
	htmlAddFileHandler("switches.html", &_switches_cb);
	htmlAddFileHandler("properties.html", &_properties_cb);
}

void server::htmlAddFileHandler(std::string_view fileName, HtmlFileCb fileCb)
{
	_fileMap[std::string(fileName)] = fileCb;
}