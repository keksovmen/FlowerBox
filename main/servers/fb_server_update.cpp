#include "fb_server_update.hpp"

#include "fb_debug.hpp"
#include "fb_update.hpp"



#define _HEADER_CONTENT_TYPE "Content-Type"
#define _CONTENT_TYPE_BOUNDARY "boundary="
#define _DATA_DELIMETER "\r\n\r\n"
#define _FILE_UPDATE "update.html"



using namespace fb;
using namespace server;

using DataCb = bool(*)(const char* data, int size);



static const char* TAG = "fb_server_update";



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
				if(at != NULL){
					valid_len = at - out;
				}else{
					//в случаи когда данные содержат в себе \0 и не являются строками
					valid_len = received - separator_len;
				}

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

			if(!cb(out, valid_len)){
				FB_DEBUG_TAG_LOG("Callback returned false, so aborting file input");
				httpd_resp_send_err(r, HTTPD_500_INTERNAL_SERVER_ERROR, "Failed to consume data in callback");
				return ESP_FAIL;
			}

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



static esp_err_t _update_cb(httpd_req_t* r)
{
	FB_DEBUG_TAG_ENTER();

	if(!update::begin()){
		httpd_resp_send_err(r, HTTPD_500_INTERNAL_SERVER_ERROR, "Failed to start ota");
		return ESP_FAIL;
	}

	esp_err_t err = _multipartFileInputHandler(r, [](const char* data, int size){
		// FB_DEBUG_TAG_LOG("Data block of size %d:\n%.*s", size, size, data);
		return update::writeSequential(data, size);
	});

	if(err == ESP_OK){
		if(update::end()){
			err |= httpd_resp_sendstr(r, "<h1>Success</h1>");
		}else{
			err |= httpd_resp_send_err(r, HTTPD_500_INTERNAL_SERVER_ERROR, "Failed to end ota update");
		}
	}

	FB_DEBUG_TAG_EXIT();

	return err;
}



void server::registerServerUpdate(Builder& builder)
{
	builder.addEndpoint(Endpoint{"/" _FILE_UPDATE, EndpointMethod::POST, nullptr, &_update_cb});
}