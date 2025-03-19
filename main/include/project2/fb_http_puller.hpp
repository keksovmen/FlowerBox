#pragma once



#include <string_view>

#include "fb_debug.hpp"
#include "fb_event_manager.hpp"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "esp_http_client.h"



namespace fb
{
	namespace project
	{
		class HttpPuller : public debug::Named, public event::EventListener
		{
			public:
				// HttpPuller();

				virtual const char* getName() const override;
				//listen for wifi status state
				virtual void handleEvent(const event::Event& event) override;

				void setUrl(std::string_view str);
				std::string_view getUrl() const;

				void start();

			private:
				std::string _url;
				bool _isWifiConnected = false;
				TaskHandle_t _task = NULL;
				esp_http_client_handle_t _httpClient = NULL;
				char _responseBuff[256];
				int _length = 0; 

				/*
				create task at startup, it will handle requests and current wifi state
				first check current state if it allows to make request then proceed
				else wait some delay repeat
				Task won't exit
				*/


				void _onWifiConnected();
				void _onWifiDisconnected();

				bool _onPerformRequest();

				//when get 404
				void _onFailure();

				//when get 200
				void _onSuccess(std::string_view data);

				void _initRequest();
				void _deinitRequest();

				//task
				static void _taskFunc(void* arg);
				static esp_err_t _requestHandler(esp_http_client_event_t *evt);
		};
	}
}