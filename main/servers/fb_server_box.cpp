#include "fb_server_box.hpp"

#include "fb_debug.hpp"
#include "fb_globals.hpp"
#include "fb_data_entry.hpp"



#define _BOX_PATH "/info"
#define _PROPERTY_PATH "/property/*"
#define _SENSOR_PATH "/sensor/*"
#define _SWITCH_PATH "/switch/*"



using namespace fb;
using namespace server;



static const char* TAG = "fb_server_box";



static esp_err_t _box_cb(httpd_req_t* r)
{
	FB_DEBUG_TAG_ENTER();

	esp_err_t err = ESP_OK;

	auto result = global::getFlowerBox()->toJson();
	FB_DEBUG_TAG_LOG("Sending FlowerBox: %s", result.c_str());

	httpd_resp_set_type(r, HTTPD_TYPE_JSON);
	
	err = httpd_resp_send(r, result.c_str(), result.length());

	FB_DEBUG_TAG_EXIT();

	return err;
}



static int _id_from_info_path(const char* path)
{
	const auto uri = std::string(path);
	const int id = std::atoi(uri.substr(uri.find_last_of("/") + 1).c_str());

	return id;
}

static int _id_from_data_path(const char* path)
{
	const auto uri = std::string(path);
	const int id = std::atoi(
		uri.substr(uri.find_last_of("/", uri.find_last_of("/") - 1) + 1).c_str());

	return id;
}

static int _timestamp_from_data_path(const char* path)
{
	return _id_from_info_path(path);
}



static esp_err_t _property_get_cb(httpd_req_t* r)
{
	FB_DEBUG_TAG_ENTER();

	esp_err_t err = ESP_OK;

	const int id = _id_from_info_path(r->uri);

	FB_DEBUG_TAG_LOG("Requesting PropertyInfo with id: %d", id);

	const auto* prop = global::getFlowerBox()->getProperty(id);
	const auto result = prop->toJson();
	FB_DEBUG_TAG_LOG("Sending PropertyInfo: %s", result.c_str());

	httpd_resp_set_type(r, HTTPD_TYPE_JSON);
	
	err = httpd_resp_send(r, result.c_str(), result.length());

	FB_DEBUG_TAG_EXIT();

	return err;
}

static esp_err_t _property_set_cb(httpd_req_t* r)
{
FB_DEBUG_TAG_ENTER();

	esp_err_t err = ESP_OK;

	const int id = _id_from_data_path(r->uri);

	FB_DEBUG_TAG_LOG("Requesting set property value with id: %d", id);

	if(strstr(r->uri, "set") == NULL){
		FB_DEBUG_TAG_LOG("Not set request");
		err = httpd_resp_send_500(r);

		return err;
	}

	char tmp[256];
	assert(r->content_len <= sizeof(tmp));

	int read = 0;
	int step = 0;
	do{
		step = httpd_req_recv(r, tmp + read, sizeof(tmp) - read);
		if(step == HTTPD_SOCK_ERR_TIMEOUT){
			continue;
		}

		if(step < 0){
			FB_DEBUG_TAG_LOG("Failed to read body");
			err = httpd_resp_send_500(r);
			break;
		}

		read += step;
	}while(read != r->content_len);

	tmp[read] = 0;

	char result[64];
	memset(result, 0, sizeof(result));

	err = httpd_query_key_value(tmp, "value", result, sizeof(result));
	if(err != ESP_OK){
		FB_DEBUG_TAG_LOG("Failed to find value key");
		err = httpd_resp_send_500(r);

	}else{
		FB_DEBUG_TAG_LOG("Setting property with id %d to %s", id, result);
		auto* prop = global::getFlowerBox()->getProperty(id);
		if(prop->setFromJson(std::string(result))){
			err = httpd_resp_send(r, nullptr, 0);
		}else{
			err = httpd_resp_send_500(r);
		}
	}

	FB_DEBUG_TAG_EXIT();

	return err;
}

static esp_err_t _sensor_info_cb(httpd_req_t* r)
{
	FB_DEBUG_TAG_ENTER();

	esp_err_t err = ESP_OK;

	const int id = _id_from_info_path(r->uri);

	FB_DEBUG_TAG_LOG("Requesting SensorInfo with id: %d", id);

	const auto* prop = global::getFlowerBox()->getSensor(id);
	const auto result = prop->toJson();
	FB_DEBUG_TAG_LOG("Sending SensorInfo: %s", result.c_str());

	httpd_resp_set_type(r, HTTPD_TYPE_JSON);
	
	err = httpd_resp_send(r, result.c_str(), result.length());

	FB_DEBUG_TAG_EXIT();

	return err;
}

static esp_err_t _switch_info_cb(httpd_req_t* r)
{
	FB_DEBUG_TAG_ENTER();

	esp_err_t err = ESP_OK;

	const int id = _id_from_info_path(r->uri);

	FB_DEBUG_TAG_LOG("Requesting SwitchInfo with id: %d", id);

	const auto* prop = global::getFlowerBox()->getSwitch(id);
	const auto result = prop->toJson();
	FB_DEBUG_TAG_LOG("Sending SwitchInfo: %s", result.c_str());

	httpd_resp_set_type(r, HTTPD_TYPE_JSON);
	
	err = httpd_resp_send(r, result.c_str(), result.length());

	FB_DEBUG_TAG_EXIT();

	return err;
}

static esp_err_t _sensor_data_cb(httpd_req_t* r)
{
	FB_DEBUG_TAG_ENTER();

	esp_err_t err = ESP_OK;

	const int id = _id_from_data_path(r->uri);
	const clock::Timestamp timestamp = _timestamp_from_data_path(r->uri);

	FB_DEBUG_TAG_LOG("Requesting Sensor data with id: %d, and timestamp: %lld", id, timestamp);

	box::DataEntry entry(228);
	const auto result = entry.toJson();

	FB_DEBUG_TAG_LOG("Sending DataEntry: %s", result.c_str());
	
	httpd_resp_set_type(r, HTTPD_TYPE_JSON);

	err |= httpd_resp_sendstr_chunk(r, "[");
	err |= httpd_resp_send_chunk(r, result.c_str(), result.length());
	err |= httpd_resp_sendstr_chunk(r, "]");
	err |= httpd_resp_sendstr_chunk(r, nullptr);

	FB_DEBUG_TAG_EXIT();

	return err;
}

static esp_err_t _switch_data_cb(httpd_req_t* r)
{
	FB_DEBUG_TAG_ENTER();

	esp_err_t err = ESP_OK;

	const int id = _id_from_data_path(r->uri);
	const clock::Timestamp timestamp = _timestamp_from_data_path(r->uri);

	FB_DEBUG_TAG_LOG("Requesting Switch data with id: %d, and timestamp: %lld", id, timestamp);

	box::DataEntry entry(228);
	const auto result = entry.toJson();

	FB_DEBUG_TAG_LOG("Sending DataEntry: %s", result.c_str());

	httpd_resp_set_type(r, HTTPD_TYPE_JSON);
	
	err |= httpd_resp_sendstr_chunk(r, "[");
	err |= httpd_resp_send_chunk(r, result.c_str(), result.length());
	err |= httpd_resp_sendstr_chunk(r, "]");
	err |= httpd_resp_sendstr_chunk(r, nullptr);

	FB_DEBUG_TAG_EXIT();

	return err;
}



static bool _is_data_request(httpd_req_t* r)
{
	return std::string(r->uri).contains("data");
}

static esp_err_t _switch_cb(httpd_req_t* r)
{
	return _is_data_request(r) ? _switch_data_cb(r) : _switch_info_cb(r);
}

static esp_err_t _sensor_cb(httpd_req_t* r)
{
	return _is_data_request(r) ? _sensor_data_cb(r) : _sensor_info_cb(r);
}



void server::registerServerBox(Builder& builder)
{
	//TODO: add endpoints for each part of andoird api
	//TODO: URL for this api must start from some preffix
	builder.addEndpoint(Endpoint{_BOX_PATH, EndpointMethod::GET, nullptr, &_box_cb});

	builder.addEndpoint(Endpoint{_PROPERTY_PATH, EndpointMethod::GET, nullptr, &_property_get_cb});
	builder.addEndpoint(Endpoint{_PROPERTY_PATH, EndpointMethod::POST, nullptr, &_property_set_cb});

	builder.addEndpoint(Endpoint{_SENSOR_PATH, EndpointMethod::GET, nullptr, &_sensor_cb});
	builder.addEndpoint(Endpoint{_SWITCH_PATH, EndpointMethod::GET, nullptr, &_switch_cb});

}
