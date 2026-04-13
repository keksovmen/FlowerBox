#include "fb_input_matrix_hw_obj.hpp"

#include "fb_db135_pwm.hpp"
#include "fb_globals.hpp"
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



static void _matrixTask(void* arg)
{
	std::array<bool, (sizeof(pins::PIN_ROW) / sizeof(pins::PIN_ROW[0])) * (sizeof(pins::PIN_COLUMN) / sizeof(pins::PIN_COLUMN[0]))> states;

	for(;;)
	{
		for(int y = 0; y < sizeof(pins::PIN_ROW) / sizeof(pins::PIN_ROW[0]); y++)
		{
			const gpio_num_t out = static_cast<gpio_num_t>(pins::PIN_ROW[y]);
			//first drive low
			gpio_set_level(out, 0);
			//now wait for capacitance to discharge
			vTaskDelay(pdMS_TO_TICKS(1));

			//then read input pin if all others pins are low, we should see pulled down as default
			//if we see high than it is from out pin
			// bool isPressed = false;
			//just to allow for io capacitance and wire capacitance to charge discharge
			// for(int i = 0; i < 10; i++){
			// 	isPressed = gpio_get_level(in);
			// }
			for(int x = 0; x < sizeof(pins::PIN_COLUMN) / sizeof(pins::PIN_COLUMN[0]); x++)
			{
				const gpio_num_t in = static_cast<gpio_num_t>(pins::PIN_COLUMN[x]);
				const bool isPressed = !gpio_get_level(in);

				const int index = y * (sizeof(pins::PIN_COLUMN) / sizeof(pins::PIN_COLUMN[0])) + x;

				if(states[index] && !isPressed){
					FB_DEBUG_LOG_I_TAG("Released button: [%d; %d]/ R:%d - C:%d", x, y, (int)out, (int)in);
				}

				states[index] = isPressed;
			}
			
			//return to default state
			gpio_set_level(out, 1);

			//now wait for capacitors to charge up
			vTaskDelay(pdMS_TO_TICKS(1));
		}
	}

	vTaskDelete(NULL);
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

	for(int in : pins::PIN_COLUMN){
		gpio_set_direction(static_cast<gpio_num_t>(in), GPIO_MODE_INPUT);
		// gpio_pulldown_en(in);
	}

	for(int out : pins::PIN_ROW){
		gpio_config_t cfg = {
			.pin_bit_mask = 1ull << out,
			.mode = GPIO_MODE_OUTPUT,
			.pull_up_en = GPIO_PULLUP_DISABLE,
			.pull_down_en = GPIO_PULLDOWN_DISABLE,
			.intr_type = GPIO_INTR_DISABLE,
		};
		gpio_config(&cfg);
		// gpio_set_direction(static_cast<gpio_num_t>(out), GPIO_MODE_OUTPUT);
		// gpio_reset_pin(static_cast<gpio_num_t>(out));
	}

	xTaskCreate(&_matrixTask, "MatrixReader", 4 * 1024, nullptr, 10, NULL);
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