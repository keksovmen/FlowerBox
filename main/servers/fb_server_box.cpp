#include "fb_server_box.hpp"

#include "fb_debug.hpp"
#include "fb_globals.hpp"



#define _URL_PATH "/info"



using namespace fb;
using namespace server;



static const char* TAG = "fb_server_box";



static esp_err_t _box_cb(httpd_req_t* r)
{
	FB_DEBUG_TAG_ENTER();

	esp_err_t err = ESP_OK;

	auto result = global::getFlowerBox()->toJson();
	FB_DEBUG_TAG_LOG("Sending FlowerBox: %s", result.c_str());
	
	err = httpd_resp_send(r, result.c_str(), result.length());

	FB_DEBUG_TAG_EXIT();

	return err;
}



static int _get_id_from_path(const char* path)
{
	const auto uri = std::string(path);
	const int id = std::atoi(uri.substr(uri.find_last_of("/", 0)).c_str());

	return id;
}

static esp_err_t _property_cb(httpd_req_t* r)
{
	FB_DEBUG_TAG_ENTER();

	esp_err_t err = ESP_OK;

	const int id = _get_id_from_path(r->uri);

	FB_DEBUG_TAG_LOG("Requesting PropertyInfo with id: %d", id);

	const auto* prop = global::getFlowerBox()->getProperty(id);
	const auto result = prop->toJson();
	FB_DEBUG_TAG_LOG("Sending PropertyInfo: %s", result.c_str());
	
	err = httpd_resp_send(r, result.c_str(), result.length());

	FB_DEBUG_TAG_EXIT();

	return err;
}

static esp_err_t _sensor_cb(httpd_req_t* r)
{
	FB_DEBUG_TAG_ENTER();

	esp_err_t err = ESP_OK;

	const int id = _get_id_from_path(r->uri);

	FB_DEBUG_TAG_LOG("Requesting SensorInfo with id: %d", id);

	const auto* prop = global::getFlowerBox()->getSensor(id);
	const auto result = prop->toJson();
	FB_DEBUG_TAG_LOG("Sending SensorInfo: %s", result.c_str());
	
	err = httpd_resp_send(r, result.c_str(), result.length());

	FB_DEBUG_TAG_EXIT();

	return err;
}

void server::registerServerBox(Builder& builder)
{
	//TODO: add endpoints for each part of andoird api
	//TODO: URL for this api must start from some preffix
	builder.addEndpoint(Endpoint{_URL_PATH, EndpointMethod::GET, nullptr, &_box_cb});
	builder.addEndpoint(Endpoint{"/property/*", EndpointMethod::GET, nullptr, &_property_cb});
	builder.addEndpoint(Endpoint{"/sensor/*", EndpointMethod::GET, nullptr, &_sensor_cb});
}
