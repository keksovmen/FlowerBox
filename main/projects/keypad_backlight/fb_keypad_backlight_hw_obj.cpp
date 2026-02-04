#include "fb_keypad_backlight_hw_obj.hpp"

#include "fb_globals.hpp"
#include "fb_json_util.hpp"
#include "fb_keyboard_handler.hpp"
#include "fb_keypad_backlight_pins.hpp"
#include "fb_keypad_backlight_settings.hpp"
#include "fb_sensor_keyboard.hpp"
#include "fb_mqtt_client.hpp"



#define _MQTT_PATH_PORT ("/keyboard/" + std::to_string(settings::getMqttId()) + "/port")



using namespace fb;
using namespace project;



static const char* TAG = "HW";



static sensor::SensorService _sensorService;
static switches::SwitchService _swithService;

static sensor::SensorStorage _sensorStorage;

static keyboard::KeyboardHandler _keyboardHandler;

static sensor::KeyboardSensor<1> _keyboardSensor({std::pair{pins::PIN_KEYBOARD_RESET, h::ButtonVK::VK_0}});
static wrappers::WrapperDb135 _db135(pins::PIN_SCL, pins::PIN_MOSI, pins::PIN_SC);
static periph::MqttClient _mqtt;

static std::array<int, 16> _outputState;
static std::array<std::pair<bool, int>, 16> _dutyState;



static void _dimmerTask(void* data)
{
	FB_DEBUG_LOG_I_TAG("Dimmer task is started");

	const int RESOLUTION = 12;
	const int BLINK_MULTIPLIER = 4;

	_dutyState.fill({false, RESOLUTION});

	uint32_t cycle = 0;

	for(;;){
		int state = 0;
		for(size_t i = 0; i < _outputState.size(); i++){
			if(_outputState[i] == 0){
				//OFF
			}else if(_outputState[i] == 1){
				//FULL ON
				state += 1 << i;
			}else{
				//BLINK
				auto& duty = _dutyState[i];
				if(cycle % RESOLUTION < duty.second){
					state += 1 << i;	
				}

				//TODO: accumulate for each pin independently, if it is needed btw
				if((cycle % RESOLUTION * BLINK_MULTIPLIER) == 0){
					if(duty.first){
						duty.second++;
						if(duty.second >= RESOLUTION){
							duty.first = false;
						}
					}else{
						duty.second--;
						if(duty.second <= 0){
							duty.first = true;
						}
					}
				}
			}
		}

		cycle++;
		_db135.setValue(state);
		vTaskDelay(pdMS_TO_TICKS(1));
	}
	vTaskDelete(NULL);
}



static void _init_from_settings()
{

}



static void _handlePortTopic(std::string_view data)
{
	int i = 0;
	const bool result = json_util::parseNumberArray(data, "pins", [&i](int val){
		_outputState[i] = val;
		i++;
		if(i >= _outputState.size()){
			i = 0;
		}
	});

	if(!result){
		FB_DEBUG_LOG_E_TAG("Failed to parse values!");
	}
}

static void _dataHandler(std::string_view topic, std::string_view data)
{
	//parse json data and see if you must fire
	FB_DEBUG_LOG_I_TAG("Data handler: %.*s -> %.*s", topic.length(), topic.cbegin(), data.length(), data.cbegin());

	if(topic == _MQTT_PATH_PORT){
		_handlePortTopic(data);
	}else{
		FB_DEBUG_LOG_W_TAG("Unexpected MQTT topic!");
	}
}



void project::initHwObjs()
{
	_sensorService.addSensor(&_keyboardSensor);

	_init_from_settings();

	_db135.init();

	_mqtt.init(settings::getIp(), settings::getPort());
	_mqtt.addDataHandler(&_dataHandler);
	_mqtt.registerSubscribeHandler([](const auto& handler){
		std::invoke(handler, _MQTT_PATH_PORT, 2);
	});

	//register key handler for dropping WIFI settings
	global::getEventManager()->attachListener(&_keyboardHandler);
	global::getEventManager()->attachListener(&_mqtt);

	xTaskCreate(&_dimmerTask, "Dimmer", 4 * 1024, nullptr, 17, NULL);
}

sensor::SensorService& project::getHwSensorService()
{
	return _sensorService;
}

switches::SwitchService& project::getHwSwitchService()
{
	return _swithService;
}

sensor::SensorStorage& project::getHwSensorStorage()
{
	return _sensorStorage;
}

wrappers::WrapperDb135& project::getHwWrapperDb()
{
	return _db135;
}