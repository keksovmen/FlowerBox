#pragma once



#include "fb_http_puller.hpp"

#include "esp_http_client.h"



namespace fb
{
	namespace project
	{
		class HttpPuller : public AbstractHttpPuller
		{
			public:
				HttpPuller(ActionCb onResult);

				virtual const char* getName() const override;
			private:
				esp_http_client_handle_t _httpClient = NULL;
				char _responseBuff[512];
				int _length = 0; 

				/*
				create task at startup, it will handle requests and current wifi state
				first check current state if it allows to make request then proceed
				else wait some delay repeat
				Task won't exit
				*/


				virtual bool _onPerformRequest() override;
				virtual void _initRequest() override;
				virtual void _deinitRequest() override;

				//task
				static esp_err_t _requestHandler(esp_http_client_event_t *evt);
		};
	}
}