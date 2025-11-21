#pragma once



#include <string_view>
#include <optional>

#include "fb_debug.hpp"
#include "fb_event_manager.hpp"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"



namespace fb
{
	namespace project
	{
		class AbstractHttpPuller : public debug::Named, public event::EventListener
		{
			public:
				//if optional has value -> success, otherwise failed request
				using ActionCb = std::function<void(std::optional<std::string_view>)>;



				AbstractHttpPuller(ActionCb onResult);

				//listen for wifi status state
				virtual void handleEvent(const event::Event& event) override;

				void start();

				void setUrl(std::string_view str);
				std::string_view getUrl() const;

				void setPause(bool state);
				void setTimeoutMs(int ms);
				
				bool isWorking() const;
				int getTimeoutMs() const;
			
			protected:
				//when get 404
				void _onFailure();
				//when get 200
				void _onSuccess(std::string_view data);

			private:
				ActionCb _actionCb;
				std::string _url;
				TaskHandle_t _task = NULL;
				int _timeoutMs = 300;
				bool _isWifiConnected = false;
				bool _pause = false;



				static void _taskFunc(void* arg);

				virtual void _initRequest() = 0;
				virtual void _deinitRequest() = 0;
				virtual bool _onPerformRequest() = 0;

				void _onWifiConnected();
				void _onWifiDisconnected();
		};
	}
}