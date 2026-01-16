#pragma once



#include <string>

#include "fb_debug.hpp"

#include "esp_http_client.h"



namespace fb
{
	namespace periph
	{
		class HttpRequest : public debug::Named
		{
			public:
				virtual const char* getName() const override;

				void init();
				void post(std::string url, std::string data);

			private:
				esp_http_client_handle_t _httpClient = NULL;
		};
	}
}