#pragma once



#include "fb_debug.hpp"
#include "fb_event_manager.hpp"

#include "esp_event_base.h"
#include "mqtt_client.h"



namespace fb
{
	namespace periph
	{
		class MqttClient : public debug::Named, public event::EventListener
		{
			public:
				using DataHandler = std::function<void(std::string_view topic, std::string_view data)>;



				MqttClient();

				virtual const char* getName() const override;
				virtual void handleEvent(const event::Event& event) override;

				void init(std::string_view uri);

				//add subscribe action with callback handler for data
				//add publish action
				void publish(std::string_view topic, std::string_view data);
				void addDataHandler(const DataHandler& handler);
			
			private:
				esp_mqtt_client_handle_t _handle;
				DataHandler _dataHandler;



				void _onBoot();
				void _onWifiConnected();
				void _onWifiDisconnected();

				static void _handler(void *handler_args, esp_event_base_t base, int32_t event_id, void *event_data);
		};
	}
}