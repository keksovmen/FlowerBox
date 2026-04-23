#include "fb_input_matrix_hw_obj.hpp"

#include "fb_db135_pwm.hpp"
#include "fb_globals.hpp"
#include "fb_input_grid.hpp"
#include "fb_input_matrix_pins.hpp"
#include "fb_input_matrix_settings.hpp"
#include "fb_json_util.hpp"
#include "fb_keyboard_handler.hpp"
#include "fb_mqtt_client.hpp"
#include "fb_sensor_keyboard.hpp"
#include "fb_static_queue.hpp"



#define _MQTT_PATH_LIGHT ("/input_matrix/" + std::to_string(settings::getMqttId()) + "/light")
#define _MQTT_PATH_TARGET ("/input_matrix/" + std::to_string(settings::getMqttId()) + "/target")
#define _MQTT_PATH_TOGGLE ("/input_matrix/" + std::to_string(settings::getMqttId()) + "/toggle")
#define _MQTT_PATH_NOTIFICATION ("/input_matrix/" + std::to_string(settings::getMqttId()) + "/notification")
#define _MQTT_PATH_BUTTONS ("/input_matrix/" + std::to_string(settings::getMqttId()) + "/buttons")



using namespace fb;
using namespace project;



static const char* TAG = "HW";
static const int _MATRIX_SIZE = pins::PIN_ROW.size() * pins::PIN_COLUMN.size();



static sensor::SensorService _sensorService;
static switches::SwitchService _swithService;

static sensor::SensorStorage _sensorStorage;

static keyboard::KeyboardHandler _keyboardHandler;
static sensor::KeyboardSensor<1> _keyboardSensor({std::pair{pins::PIN_KEYBOARD_RESET, h::ButtonVK::VK_0}});
static DRAM_ATTR wrappers::WrapperDb135<2> _db135(pins::PIN_SCL, pins::PIN_MOSI, pins::PIN_SC);
static fb::util::Db135_Pwm<2> _dbPwm(_db135);
static periph::MqttClient _mqtt;
static fb::util::InputGrid<pins::PIN_ROW.size(), pins::PIN_COLUMN.size()> _inputMatrix(1, 1, pins::PIN_ROW, pins::PIN_COLUMN);

static std::array<bool, _MATRIX_SIZE> _toggleStates;
static std::array<bool, _MATRIX_SIZE> _targetState;

static fb::util::StaticQueue<20, decltype(_inputMatrix)::ButtonEntry> _buttonsQueue;



static void _mqttPublishTask(void* arg)
{
	for(;;)
	{
		auto data = _buttonsQueue.readQueue();
		if(data.empty()){
			vTaskDelay(pdMS_TO_TICKS(10));
			continue;
		}

		cJSON* root = cJSON_CreateObject();
		cJSON* arr = cJSON_AddArrayToObject(root, "data");
		//send data
		for(const auto& e : data)
		{
			//convert to json
			cJSON* obj = cJSON_CreateObject();
			cJSON_AddNumberToObject(obj, "i", e.index);
			cJSON_AddNumberToObject(obj, "mov", (int) e.action);
			cJSON_AddNumberToObject(obj, "ms", e.durationMs);
			cJSON_AddItemToArray(arr, obj);
		}

		char* jsonStr = cJSON_PrintUnformatted(arr);
		cJSON_Delete(root);

		FB_DEBUG_LOG_I_TAG("Sending buttons: %s", jsonStr);
		
		_mqtt.publish(_MQTT_PATH_BUTTONS, jsonStr);

		cJSON_free(jsonStr);
	}

	vTaskDelete(NULL);
}



static bool _checkTargetCondition()
{
	//compare to what needed and if good, send mqtt and start blinking on all sequence
	if(_toggleStates == _targetState){
		for(int i = 0; i < _MATRIX_SIZE; i++){
			_dbPwm.setMode(i, _toggleStates[i] ? decltype(_dbPwm)::Mode::BLINK : decltype(_dbPwm)::Mode::OFF);
		}

		_mqtt.publish(_MQTT_PATH_NOTIFICATION, "Finished");

		return true;
	}

	return false;
}

static void _handlePortTopic(std::string_view data)
{
	int i = 0;
	const bool result = json_util::parseNumberArray(data, "data", [&i](int val){
		_dbPwm.setMode(i, static_cast<decltype(_dbPwm)::Mode>(val));
		i++;
	});

	if(!result){
		FB_DEBUG_LOG_E_TAG("Failed to parse values!");
	}
}

static void _handleTargetTopic(std::string_view data)
{
	int i = 0;
	const bool result = json_util::parseNumberArray(data, "data", [&i](int val){
		if(i >= _MATRIX_SIZE){
			FB_DEBUG_LOG_W_TAG("Illegal index");
			return;
		}
		_targetState[i] = val != 0;
		i++;
	});

	if(!result){
		FB_DEBUG_LOG_E_TAG("Failed to parse values!");
	}

	//save to settings
	std::string str = "";
	for(auto val : _targetState){
		str += val ? "1" : "0";
	}
	
	settings::setCombination(str);
}

static void _handleToggleTopic(std::string_view data)
{
	int i = 0;
	const bool result = json_util::parseNumberArray(data, "data", [&i](int val){
		if(i >= _MATRIX_SIZE){
			FB_DEBUG_LOG_W_TAG("Illegal index");
			return;
		}
		_toggleStates[i] = val != 0;
		i++;
	});

	if(!result){
		FB_DEBUG_LOG_E_TAG("Failed to parse values!");
	}

	if(!_checkTargetCondition()){
		//apply to visual
		for(int i = 0; i < _MATRIX_SIZE; i++){
			_dbPwm.setMode(i, _toggleStates[i] ? decltype(_dbPwm)::Mode::ON : decltype(_dbPwm)::Mode::OFF);
		}
	}
}



static void _dataHandler(std::string_view topic, std::string_view data)
{
	//parse json data and see if you must fire
	FB_DEBUG_LOG_I_TAG("Data handler: %.*s -> %.*s", topic.length(), topic.cbegin(), data.length(), data.cbegin());

	if(topic == _MQTT_PATH_LIGHT){
		_handlePortTopic(data);

	}else if(topic == _MQTT_PATH_TARGET){
		_handleTargetTopic(data);

	}else if(topic == _MQTT_PATH_TOGGLE){
		_handleToggleTopic(data);

	}else{
		FB_DEBUG_LOG_W_TAG("Unexpected MQTT topic!");
	}
}



static void _handleButtonPresses(decltype(_inputMatrix)::ButtonEntry button)
{
	const int index = button.index;
	_toggleStates[index] = !_toggleStates[index];
	
	_buttonsQueue.addEntry(button);

	if(!_checkTargetCondition()){
		_dbPwm.setMode(index, _toggleStates[index] ? decltype(_dbPwm)::Mode::ON : decltype(_dbPwm)::Mode::OFF);
	}
}



static void _init_from_settings()
{
	_dbPwm.startTask(settings::getPulseTime(), 17);

	const auto comb = settings::getCombination();
	for(int i = 0; i < std::min((int) comb.length(), _MATRIX_SIZE); i++){
		_targetState[i] = comb[i] != '0';
	}
}



void project::initHwObjs()
{
	_sensorService.addSensor(&_keyboardSensor);

	_db135.init();

	_init_from_settings();

	_mqtt.init(settings::getIp(), settings::getPort(), 3 * 1024);
	_mqtt.addDataHandler(&_dataHandler);
	_mqtt.registerSubscribeHandler([](const auto& handler){
		std::invoke(handler, _MQTT_PATH_LIGHT, 2);
		std::invoke(handler, _MQTT_PATH_TARGET, 2);
		std::invoke(handler, _MQTT_PATH_TOGGLE, 2);
	});

	//register key handler for dropping WIFI settings
	global::getEventManager()->attachListener(&_keyboardHandler);
	global::getEventManager()->attachListener(&_mqtt);

	//TODO: add settings for debounce and still pressing
	_inputMatrix.init(&_handleButtonPresses, settings::getDebounceMs(), settings::getStillPeriodMs());
	_inputMatrix.start(10, 4 * 1024);

	xTaskCreate(&_mqttPublishTask, "MqttPublisher", 4 * 1024, nullptr, 10, nullptr);
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