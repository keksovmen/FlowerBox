#include "fb_keypad_backlight_hw_obj.hpp"

#include "fb_globals.hpp"
#include "fb_json_util.hpp"
#include "fb_keyboard_handler.hpp"
#include "fb_keypad_backlight_pins.hpp"
#include "fb_keypad_backlight_settings.hpp"
#include "fb_sensor_keyboard.hpp"
#include "fb_mqtt_client.hpp"

// #include "mcp23017.h"



#define _MQTT_PATH_PORT ("/keyboard/" + std::to_string(settings::getMqttId()) + "/port")



using namespace fb;
using namespace project;



static const char* TAG = "HW";



static sensor::SensorService _sensorService;
static switches::SwitchService _swithService;

static sensor::SensorStorage _sensorStorage;

static keyboard::KeyboardHandler _keyboardHandler;

// static i2c_bus_handle_t _i2cBus = nullptr;
// static mcp23017_handle_t _mcp = nullptr;

static sensor::KeyboardSensor<1> _keyboardSensor({std::pair{pins::PIN_KEYBOARD_RESET, h::ButtonVK::VK_0}});
static wrappers::WrapperDb135 _db135(pins::PIN_SCL, pins::PIN_MOSI, pins::PIN_SC);
static periph::MqttClient _mqtt;



static void _init_from_settings()
{

}



static void _handlePortTopic(std::string_view data)
{
	auto* json = cJSON_ParseWithLength(data.cbegin(), data.length());
	auto val = json_util::getIntFromJsonOrDefault(json, "val", 0);
	cJSON_Delete(json);

	_db135.setValue(val);
	FB_DEBUG_LOG_I_TAG("Set DB135 to %d", val);
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

	// i2c_config_t cfg = {
	// 	.mode = I2C_MODE_MASTER,
	// 	.sda_io_num = 6,
	// 	.scl_io_num = 7,
	// 	.sda_pullup_en = true,
	// 	.scl_pullup_en = true,
	// 	.master = {.clk_speed = 100000},
	// 	.clk_flags = I2C_CLK_SRC_DEFAULT,
	// };
	// _i2cBus = i2c_bus_create(I2C_NUM_0, &cfg);

	// //TODO: why do i need to shift right by one position to match address?
	// _mcp = mcp23017_create(_i2cBus, 0x40 >> 1);
	// mcp23017_set_io_dir(_mcp, 0x01, MCP23017_GPIOA);
	// vTaskDelay(pdMS_TO_TICKS(100));
	// mcp23017_write_io(_mcp, 0x01, MCP23017_GPIOA);
	// vTaskDelay(pdMS_TO_TICKS(100));
	// uint8_t gpios = mcp23017_read_io(_mcp, MCP23017_GPIOA);
	// FB_DEBUG_LOG_W_TAG("Read GPIOA: %u", gpios);
	// vTaskDelay(pdMS_TO_TICKS(100));

	// mcp23017_set_adc(_mcp, 0x01);
	// vTaskDelay(pdMS_TO_TICKS(100));
	
	// uint16_t adc = mcp23017_read_adc(_mcp, 0);
	// FB_DEBUG_LOG_W_TAG("Read ADC 0: %u", adc);
	_db135.init();
	// _db135.setValue(0xFFFF);
	// vTaskDelay(portMAX_DELAY);
	// mcp23017_delete(&_mcp);
	// _mcp = mcp23017_create(_i2cBus, 0x40 << 1);
	// mcp23017_set_io_dir(_mcp, 0x01, MCP23017_GPIOA);
	// mcp23017_write_io(_mcp, 0x01, MCP23017_GPIOA);

	// _mcp.setPinMode(MCP23017_PIN0, false);
	// _mcp.writePin(MCP23017_PIN0, true);

	_mqtt.init(settings::getIp(), settings::getPort());
	_mqtt.addDataHandler(&_dataHandler);
	_mqtt.registerSubscribeHandler([](const auto& handler){
		std::invoke(handler, _MQTT_PATH_PORT, 2);
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

wrappers::WrapperDb135& project::getHwWrapperDb()
{
	return _db135;
}