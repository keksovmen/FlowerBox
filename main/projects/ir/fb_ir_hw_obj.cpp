#include "fb_ir_hw_obj.hpp"

#include <array>

#include "fb_globals.hpp"
#include "fb_ir_pins.hpp"
#include "fb_ir_reader.hpp"
#include "fb_ir_settings.hpp"
#include "fb_ir_transmitter.hpp"
#include "fb_json_util.hpp"
#include "fb_keyboard_handler.hpp"
#include "fb_mqtt_client.hpp"
#include "fb_sensor_keyboard.hpp"
#include "fb_wrappers.hpp"



#define _MQTT_PATH_TRANSCEIVER(index) ("/ir/" + std::to_string(settings::getMqttId()) + "/pin/" + std::to_string(index))
#define _MQTT_PATH_ACTIVATE ("/ir/" + std::to_string(settings::getMqttId()) + "/activate")
#define _MQTT_PATH_DEACTIVATE ("/ir/" + std::to_string(settings::getMqttId()) + "/deactivate")



using namespace fb;
using namespace project;



static const char* TAG = "HW";



static sensor::SensorService _sensorService;
static switches::SwitchService _swithService;

static sensor::SensorStorage _sensorStorage;

static keyboard::KeyboardHandler _keyboardHandler;
static sensor::KeyboardSensor<1> _keyboardSensor({std::pair{pins::PIN_KEYBOARD_RESET, h::ButtonVK::VK_0}});

static DRAM_ATTR periph::IrReader _irReader;
static periph::IrTransmitter _irSender;

static periph::MqttClient _mqtt;

static std::array<bool, pins::RECEIVER_COUNT> _entries;
static std::array<wrappers::WrapperGpio, pins::RECEIVER_COUNT> _leds{{
	{static_cast<gpio_num_t>(pins::PINS_LED[0]), false},
	{static_cast<gpio_num_t>(pins::PINS_LED[1]), false},
	{static_cast<gpio_num_t>(pins::PINS_LED[2]), false},
	{static_cast<gpio_num_t>(pins::PINS_LED[3]), false},
	{static_cast<gpio_num_t>(pins::PINS_LED[4]), false}
}};

static fb::util::TimeScheduler _mqttSender;



static void _init_from_settings()
{

}



static void _enablePin(int index)
{
	if(index < 0 || index >= _entries.size()){
		return;
	}

	_entries[index] = true;
	_leds[index].setValue(true);
}

static void _disablePin(int index)
{
	if(index < 0 || index >= _entries.size()){
		return;
	}

	_entries[index] = false;
	_leds[index].setValue(false);
}



static void _mqttDataHandler(std::string_view topic, std::string_view data)
{
	//start actions
	if(topic == "/ir/action"){
		if(data == "-1"){
			_irSender.sendHeal(pins::PIN_TRANSCEIVER);

		}else if(data == "-2"){
			_irSender.sendKill(pins::PIN_TRANSCEIVER);

		}else{
			_irSender.sendAttack(pins::PIN_TRANSCEIVER, periph::Ir::AttackCmd{std::stoi(std::string(data.begin(), data.length()))});

		}
	}else if(topic == _MQTT_PATH_ACTIVATE){
		json_util::parseNumberArray(data, "pins", &_enablePin);

	}else if(topic == _MQTT_PATH_DEACTIVATE){
		json_util::parseNumberArray(data, "pins", &_disablePin);

	}else{
		// do nothing
	}
}



static std::string _serializeAttack(periph::Ir::AttackCmd cmd) {
	// Create a cJSON object
	cJSON* root = cJSON_CreateObject();
	if (root == nullptr) {
		return "{}"; // Return empty JSON on error
	}
	
	// Add integer values to the object
	const char* teams[4] = {
		"RED", "BLUE", "YELLOW", "GREEN"
	};
	cJSON_AddStringToObject(root, "team", teams[cmd.packet.team]);
	cJSON_AddNumberToObject(root, "id", cmd.packet.id);
	cJSON_AddNumberToObject(root, "dmg", cmd.packet.damage);
	
	// Convert to JSON string
	char* jsonStr = cJSON_Print(root);
	std::string result = jsonStr ? jsonStr : "{}";
	
	// Clean up
	if (jsonStr) free(jsonStr);
	cJSON_Delete(root);
	
	return result;
}

static void _readerCb(int pin, int index, uint32_t v, int length)
{
	FB_DEBUG_LOG_I_TAG("Data CB: %d, %d bits = %u", pin, length, v);
	if(length == FB_IR_COMMANDS_GLOBAL_LENGTH_BITS){
		//ignore global commands only sense attack
		FB_DEBUG_LOG_I_TAG("Global command: %s", v == FB_IR_COMMANDS_HEAL ? "HEAL" : "KILL");
		return;
	}

	if(_entries[index]){
		_mqttSender.addActionDelayed([index, v](){
			//send mqtt data that we set given pin to given value
			_mqtt.publish(_MQTT_PATH_TRANSCEIVER(index), _serializeAttack(periph::Ir::AttackCmd(static_cast<uint16_t>(v))));
		}, 0, 1000);

		//do LEAD logic
		auto turnOn = [index](){
			_leds[index].setValue(true);
		};
		auto turnOff = [index](){
			_leds[index].setValue(false);
		};
		auto validate = [index](){
			_leds[index].setValue(_entries[index]);
		};

		turnOff();
		global::getTimeScheduler()->addActionDelayed(turnOn, 50, 500);
		global::getTimeScheduler()->addActionDelayed(turnOff, 100, 500);
		global::getTimeScheduler()->addActionDelayed(turnOn, 150, 500);
		global::getTimeScheduler()->addActionDelayed(turnOff, 200, 500);
		global::getTimeScheduler()->addActionDelayed(validate, 250, 500);
	}
}



void project::initHwObjs()
{
	_sensorService.addSensor(&_keyboardSensor);

	_init_from_settings();

	_mqttSender.start();

	_entries.fill(false);
	for(auto& wrap : _leds){
		wrap.init();
	}

	_irReader.startTask(15, 4096, &_readerCb);
	_irReader.init();
	for(int i = 0; i < pins::RECEIVER_COUNT; i++){
		_irReader.addPin(pins::PINS_RECEIVER[i]);
	}

	_irSender.addGpio(pins::PIN_TRANSCEIVER);

	_mqtt.init(settings::getIp(), settings::getPort());
	_mqtt.addDataHandler(&_mqttDataHandler);
	_mqtt.registerSubscribeHandler([](auto r){
		std::invoke(r, "/ir/action", 2);
		std::invoke(r, _MQTT_PATH_ACTIVATE, 2);
		std::invoke(r, _MQTT_PATH_DEACTIVATE, 2);
	});


	//register key handler for dropping WIFI settings
	global::getEventManager()->attachListener(&_keyboardHandler);
	global::getEventManager()->attachListener(&_mqtt);
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