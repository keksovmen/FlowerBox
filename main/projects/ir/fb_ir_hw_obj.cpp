#include "fb_ir_hw_obj.hpp"

#include "fb_globals.hpp"
#include "fb_ir_pins.hpp"
#include "fb_ir_reader.hpp"
#include "fb_ir_settings.hpp"
#include "fb_ir_transmitter.hpp"
#include "fb_keyboard_handler.hpp"
#include "fb_mqtt_client.hpp"
#include "fb_sensor_keyboard.hpp"



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



static void _init_from_settings()
{

}

static void _mqttDataHandler(std::string_view topic, std::string_view data)
{
	//start actions
	if(data == "1"){
		//send heal
		_irSender.sendHeal(pins::PIN_TRANSCEIVER);
	}else if (data == "2"){
		//send kill
		_irSender.sendKill(pins::PIN_TRANSCEIVER);
	}else{
		// do nothing
	}
}


void project::initHwObjs()
{
	_sensorService.addSensor(&_keyboardSensor);

	_init_from_settings();

	_irReader.startTask(15, 4096, [](int pin, uint32_t v, int length){
		FB_DEBUG_LOG_I_TAG("Data CB: %d, %d bits = %u", pin, length, v);
	});

	_irReader.init();
	for(int i = 0; i < sizeof(pins::PINS_RECEIVER) / sizeof(pins::PINS_RECEIVER[0]); i++){
		_irReader.addPin(pins::PINS_RECEIVER[i]);
	}

	_irSender.addGpio(pins::PIN_TRANSCEIVER);

	_mqtt.init(settings::getIp(), settings::getPort());
	_mqtt.addDataHandler(&_mqttDataHandler);
	_mqtt.registerSubscribeHandler([](auto r){
		std::invoke(r, "/ir/action", 2);
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