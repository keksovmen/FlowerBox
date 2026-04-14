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



#define _MQTT_PATH_PORT ("/input_matrix/" + std::to_string(settings::getMqttId()) + "/port")



using namespace fb;
using namespace project;



static const char* TAG = "HW";



static sensor::SensorService _sensorService;
static switches::SwitchService _swithService;

static sensor::SensorStorage _sensorStorage;

static keyboard::KeyboardHandler _keyboardHandler;
static sensor::KeyboardSensor<1> _keyboardSensor({std::pair{pins::PIN_KEYBOARD_RESET, h::ButtonVK::VK_0}});
static DRAM_ATTR wrappers::WrapperDb135<2> _db135(pins::PIN_SCL, pins::PIN_MOSI, pins::PIN_SC);
static fb::util::Db135_Pwm<2> _dbPwm(_db135);
static periph::MqttClient _mqtt;
static fb::util::InputGrid<6, 5> _inputMatrix(1, 1, pins::PIN_ROW, pins::PIN_COLUMN);



static void _handlePortTopic(std::string_view data)
{
	int i = 0;
	const bool result = json_util::parseNumberArray(data, "pins", [&i](int val){
		_dbPwm.setMode(i, static_cast<decltype(_dbPwm)::Mode>(val));
		i++;
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



static void _init_from_settings()
{
	_dbPwm.startTask(settings::getPulseTime(), 17);
}



void project::initHwObjs()
{
	// _sensorService.addSensor(&_keyboardSensor);

	_db135.init();

	_init_from_settings();

	_mqtt.init(settings::getIp(), settings::getPort(), 3 * 1024);
	_mqtt.addDataHandler(&_dataHandler);
	_mqtt.registerSubscribeHandler([](const auto& handler){
		std::invoke(handler, _MQTT_PATH_PORT, 2);
	});

	//register key handler for dropping WIFI settings
	global::getEventManager()->attachListener(&_keyboardHandler);
	global::getEventManager()->attachListener(&_mqtt);

	_inputMatrix.init([](auto pair){
		//TODO: implement
	});
	_inputMatrix.start(10, 4 * 1024);
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