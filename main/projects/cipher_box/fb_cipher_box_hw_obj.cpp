#include "fb_cipher_box_hw_obj.hpp"

#include "fb_cipher_box_pins.hpp"
#include "fb_cipher_box_settings.hpp"
#include "fb_expander_matrix.hpp"
#include "fb_globals.hpp"
#include "fb_http_request.hpp"
#include "fb_i2c.hpp"
#include "fb_keyboard_handler.hpp"
#include "fb_sensor_keyboard.hpp"

#include "ex_protocol.h"



using namespace fb;
using namespace project;



static const char* TAG = "CipherBoxHW";



static sensor::SensorService _sensorService;
static switches::SwitchService _swithService;

static sensor::SensorStorage _sensorStorage;

static keyboard::KeyboardHandler _keyboardHandler;
static sensor::KeyboardSensor<1> _keyboardSensor({std::pair{pins::PIN_KEYBOARD_RESET, h::ButtonVK::VK_0}});


static interfaces::I2c _i2c;
static ex_master_t _expander;
static periph::ExpanderMatrix _matrix(_expander);
static periph::HttpRequest _http;



static void _monitorTask(void* arg)
{
	_http.init();
	auto prevState = _matrix.readPins();
	//make post request
	for(;;){
		auto state = _matrix.readPins();
		bool changedFlag = false;
		for (size_t i = 0; i < state.size(); i++)
		{
			if(abs((int) prevState[i] - (int) state[i]) > 15){
				FB_DEBUG_LOG_I_TAG("%d = %u -> %u", i, prevState[i], state[i]);
				changedFlag = true;
			}
		}

		if(changedFlag){
			changedFlag = false;
			prevState = std::move(state);
			//TODO: make POST request
			char buffer[64] = "[";
			char* ptr = &buffer[1];
			for(auto v : prevState){
				ptr = ptr + sprintf(ptr, "%d,", v);
			}
			ptr--;
			ptr[0] = ']';
			ptr[1] = '\0';

			FB_DEBUG_LOG_I_TAG("%s", buffer);
			_http.post("http://192.168.0.111:8080/loh", ptr);
		}
		
		vTaskDelay(pdMS_TO_TICKS(1000));
	}

	vTaskDelete(NULL);
}



static void _init_from_settings()
{

}



void project::initHwObjs()
{
	_sensorService.addSensor(&_keyboardSensor);

	_init_from_settings();


	//register key handler for dropping WIFI settings
	global::getEventManager()->attachListener(&_keyboardHandler);

	_i2c.init(0, pins::PIN_SDA, pins::PIN_SCL);
	_i2c.addDevice(100000, EX_PROTOCOL_DEFAULT_I2C_ADDRESS >> 1);

	ex_i2c_t masterCfg;
	masterCfg.read_cb = [](int32_t address, uint8_t* data, int32_t length){return _i2c.read(0, {data, (unsigned int) length}, 1000);};
	masterCfg.write_cb = [](int32_t address, uint8_t* data, int32_t length){return _i2c.write(0, {data, (unsigned int) length}, 1000);};
	ex_master_init(&_expander, &masterCfg);

	//TODO: update CH32 for last version and change false to true
	ex_master_set_pin_dir(&_expander, EX_MASTER_PIN_6, false);
	ex_master_set_pin_dir(&_expander, EX_MASTER_PIN_7, false);
	ex_master_set_pin_dir(&_expander, EX_MASTER_PIN_8, false);
	ex_master_set_pin_dir(&_expander, EX_MASTER_PIN_9, false);
	ex_master_set_pin_dir(&_expander, EX_MASTER_PIN_0, false);
	ex_master_set_pin_dir(&_expander, EX_MASTER_PIN_10, false);
	ex_master_set_pin_adc_mode(&_expander, EX_MASTER_ADC_PIN_0, true);
	ex_master_set_pin_adc_mode(&_expander, EX_MASTER_ADC_PIN_1, true);

	_matrix.addEntry({6, 0, true});
	_matrix.addEntry({6, 1, true});
	_matrix.addEntry({7, 0, true});
	_matrix.addEntry({7, 1, true});
	_matrix.addEntry({8, 0, true});
	_matrix.addEntry({8, 1, true});
	_matrix.addEntry({9, 0, true});
	_matrix.addEntry({9, 1, true});
	_matrix.addEntry({0, 0, true});
	_matrix.addEntry({0, 1, true});
	_matrix.addEntry({10, 0, true});
	_matrix.addEntry({10, 1, true});

	if(xTaskCreate(&_monitorTask, "mon", 4 * 1024, NULL, 8, NULL) != pdPASS){
		FB_DEBUG_LOG_E_TAG("Failed to create the monitor task");
	};
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