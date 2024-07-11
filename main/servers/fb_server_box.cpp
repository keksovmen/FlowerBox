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

void server::registerServerBox(Builder& builder)
{
	//TODO: add endpoints for each part of andoird api
	//TODO: URL for this api must start from some preffix
	builder.addEndpoint(Endpoint{_URL_PATH, EndpointMethod::GET, nullptr, &_box_cb});
}
