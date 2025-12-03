#include "fb_mqtt_client.hpp"

#include "fb_wifi.hpp"

#include "esp_pm.h"
#include "driver/gpio.h"



using namespace fb;
using namespace periph;



static void log_error_if_nonzero(const char *message, int error_code)
{
	const char* TAG = "DEBUG";

    if (error_code != 0) {
        FB_DEBUG_LOG_E_TAG( "Last error %s: 0x%x", message, error_code);
    }
}



MqttClient::MqttClient()
	: _handle(nullptr)
{
}

const char* MqttClient::getName() const
{
	return "MqttClient";
}

void MqttClient::handleEvent(const event::Event& event)
{
	if(event.groupId == event::EventGroup::BOOT){
		// _onBoot();
	}else if(event.groupId == event::EventGroup::WIFI){
		if(event.eventId == wifi::WifiEventId::CONNECTED){
			_onWifiConnected();
		}else if(event.eventId == wifi::WifiEventId::FAILED_TO_CONNECT){
			_onWifiDisconnected();
		}
	}
}

void MqttClient::init(std::string_view uri)
{
	esp_mqtt_client_config_t mqtt_cfg = {0};
	mqtt_cfg.broker.address.uri = uri.begin();

    _handle = esp_mqtt_client_init(&mqtt_cfg);
    esp_mqtt_client_register_event(_handle, MQTT_EVENT_ANY, &_handler, this);
	FB_DEBUG_LOG_W_OBJ("MQTT client created: %s", _handle == nullptr ? "False" : "True");
}

void MqttClient::publish(std::string_view topic, std::string_view data)
{
	if(!_handle){
		FB_DEBUG_LOG_E_OBJ("Have no client for publish action!");
		return;
	}

	const auto err = esp_mqtt_client_publish(_handle, topic.begin(), data.begin(), 0, 2, 0);
	if(err <= 0){
		FB_DEBUG_LOG_E_OBJ("Failed to publish!: %s = %s", topic.begin(), data.begin());
	}
}

void MqttClient::addDataHandler(const DataHandler& handler)
{
	_dataHandler = handler;
}

void MqttClient::registerSubscribeHandler(const SubscribeHandler& handler)
{
	_subscribeHandler = handler;
	if(_connected && _subscribeHandler){
		_callSubscribeHandler();	
	}
}


void MqttClient::_onBoot()
{
	// esp_mqtt_client_config_t mqtt_cfg = {0};
	// mqtt_cfg.broker.address.uri = _uri.c_str();

    // _handle = esp_mqtt_client_init(&mqtt_cfg);
    // esp_mqtt_client_register_event(_handle, MQTT_EVENT_ANY, &_handler, this);
	// FB_DEBUG_LOG_W_OBJ("MQTT client created: %s", _handle == nullptr ? "False" : "True");
}

void MqttClient::_onWifiConnected()
{
	if(_handle){
		esp_mqtt_client_start(_handle);
	}else{
		FB_DEBUG_LOG_E_OBJ("Have no client!");
	}
}

void MqttClient::_onWifiDisconnected()
{
	if(_handle){
		esp_mqtt_client_stop(_handle);
	}else{
		FB_DEBUG_LOG_E_OBJ("Have no client!");
	}
}

void MqttClient::_callSubscribeHandler()
{
	std::invoke(_subscribeHandler, [this](std::string_view topic, int qos){
		esp_mqtt_client_subscribe_single(_handle, topic.begin(), qos);
	});
}

void MqttClient::_handler(void *handler_args, esp_event_base_t base, int32_t event_id, void *event_data)
{
	MqttClient* me = static_cast<MqttClient*>(handler_args);
	const char* TAG = me->getName();

	FB_DEBUG_LOG_I_TAG("Event dispatched from event loop base=%s, event_id=%" PRIi32, base, event_id);
    esp_mqtt_event_handle_t event = static_cast<esp_mqtt_event_handle_t>(event_data);
    // esp_mqtt_client_handle_t client = event->client;
    // int msg_id;
    switch ((esp_mqtt_event_id_t)event_id)
	{
		case MQTT_EVENT_CONNECTED:
			FB_DEBUG_LOG_I_TAG("MQTT_EVENT_CONNECTED");
			me->_connected = true;
			if(me->_subscribeHandler){
				me->_callSubscribeHandler();	
			}
			// msg_id = esp_mqtt_client_subscribe(client, "/controls", 2);
			// FB_DEBUG_LOG_I_TAG("sent subscribe successful, msg_id=%d", msg_id);
			break;
		case MQTT_EVENT_DISCONNECTED:
			FB_DEBUG_LOG_I_TAG("MQTT_EVENT_DISCONNECTED");
			me->_connected = true;
			break;

		case MQTT_EVENT_SUBSCRIBED:
			FB_DEBUG_LOG_I_TAG("MQTT_EVENT_SUBSCRIBED, msg_id=%d", event->msg_id);
			break;
		case MQTT_EVENT_UNSUBSCRIBED:
			FB_DEBUG_LOG_I_TAG("MQTT_EVENT_UNSUBSCRIBED, msg_id=%d", event->msg_id);
			break;
		case MQTT_EVENT_PUBLISHED:
			FB_DEBUG_LOG_I_TAG("MQTT_EVENT_PUBLISHED, msg_id=%d", event->msg_id);
			break;
		case MQTT_EVENT_DATA:
			FB_DEBUG_LOG_I_TAG("MQTT_EVENT_DATA: %.*s = %.*s", event->topic_len, event->topic, event->data_len, event->data);
			if(me->_dataHandler){
				std::invoke(me->_dataHandler,
					std::string_view{event->topic, static_cast<std::string_view::size_type>(event->topic_len)},
					std::string_view{event->data, static_cast<std::string_view::size_type>(event->data_len)});
			}
			break;
		case MQTT_EVENT_ERROR:
			FB_DEBUG_LOG_I_TAG("MQTT_EVENT_ERROR");
			if (event->error_handle->error_type == MQTT_ERROR_TYPE_TCP_TRANSPORT) {
				log_error_if_nonzero("reported from esp-tls", event->error_handle->esp_tls_last_esp_err);
				log_error_if_nonzero("reported from tls stack", event->error_handle->esp_tls_stack_err);
				log_error_if_nonzero("captured as transport's socket errno",  event->error_handle->esp_transport_sock_errno);
				FB_DEBUG_LOG_I_TAG("Last errno string (%s)", strerror(event->error_handle->esp_transport_sock_errno));
			}
			break;
		default:
			FB_DEBUG_LOG_I_TAG("Other event id:%d", event->event_id);
			break;
    }
}
