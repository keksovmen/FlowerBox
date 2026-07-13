#include "fb_cipher_box_hw_obj.hpp"

#include "fb_cipher_box_pins.hpp"
#include "fb_cipher_box_settings.hpp"
#include "fb_expander_matrix.hpp"
#include "fb_globals.hpp"
#include "fb_http_request.hpp"
#include "fb_i2c.hpp"
#include "fb_keyboard_handler.hpp"
#include "fb_sensor_keyboard.hpp"
#include "fb_mqtt_client.hpp"

#include "ex_protocol.h"

#include "esp_timer.h"



using namespace fb;
using namespace project;



#define _MQTT_PATH_STATE ("/cipher_box/" + std::to_string(settings::getMqttId()) + "/state")



struct _Entry
{
	int64_t timestampUs = 0;
	bool sended = false;
};



static const char* TAG = "CipherBoxHW";



static sensor::SensorService _sensorService;
static switches::SwitchService _swithService;

static sensor::SensorStorage _sensorStorage;

static keyboard::KeyboardHandler _keyboardHandler;
static sensor::KeyboardSensor<1> _keyboardSensor({std::pair{pins::PIN_KEYBOARD_RESET, h::ButtonVK::VK_0}});

static interfaces::I2c _i2c;
static ex_master_t _expander;

#if _HW_VERSION == 2
	static periph::ExpanderMatrixByMultiplexer _matrix(_expander, settings::getReadingsCount());
#else
	static periph::ExpanderMatrix _matrix(_expander);
#endif
static periph::HttpRequest _http;
static periph::MqttClient _mqtt;

static bool _i2cErrorFlag = false;



static void _initExpander()
{
	#if _HW_VERSION == 2
		//HW_2
		ex_master_set_pin_dir(&_expander, EX_MASTER_PIN_2, true);
		ex_master_set_pin_dir(&_expander, EX_MASTER_PIN_15, true);
		ex_master_set_pin_dir(&_expander, EX_MASTER_PIN_14, true);
		ex_master_set_pin_adc_mode(&_expander, EX_MASTER_ADC_PIN_6, true);

		if(settings::getChipsCount() > 1){
			ex_master_set_pin_dir(&_expander, EX_MASTER_PIN_7, true);
			ex_master_set_pin_dir(&_expander, EX_MASTER_PIN_6, true);
			ex_master_set_pin_dir(&_expander, EX_MASTER_PIN_5, true);
			ex_master_set_pin_adc_mode(&_expander, EX_MASTER_ADC_PIN_0, true);
		}
		
		if(settings::getChipsCount() > 2){
			ex_master_set_pin_dir(&_expander, EX_MASTER_PIN_10, true);
			ex_master_set_pin_dir(&_expander, EX_MASTER_PIN_0, true);
			ex_master_set_pin_dir(&_expander, EX_MASTER_PIN_8, true);
			ex_master_set_pin_adc_mode(&_expander, EX_MASTER_ADC_PIN_4, true);
		}
	#else
		//HW_1
		ex_master_set_pin_dir(&_expander, EX_MASTER_PIN_6, true);
		ex_master_set_pin_dir(&_expander, EX_MASTER_PIN_7, true);
		ex_master_set_pin_dir(&_expander, EX_MASTER_PIN_8, true);
		ex_master_set_pin_dir(&_expander, EX_MASTER_PIN_9, true);
		ex_master_set_pin_dir(&_expander, EX_MASTER_PIN_0, true);
		ex_master_set_pin_dir(&_expander, EX_MASTER_PIN_10, true);
		ex_master_set_pin_adc_mode(&_expander, EX_MASTER_ADC_PIN_0, true);
		ex_master_set_pin_adc_mode(&_expander, EX_MASTER_ADC_PIN_1, true);
	#endif
}



static void _monitorTask(void* arg)
{
	_http.init();
	auto prevState = _matrix.readPins();
	std::vector<_Entry> timeStates(prevState.size());
	const int error = settings::getDeltaError();
	const int bounceTimeUs = settings::getBounceTimeMs() * 1000;

	bool changedFlag = false;

	for(;;){
		auto state = _matrix.readPins();
		if(_i2cErrorFlag){
			vTaskDelay(pdMS_TO_TICKS(100));
			FB_DEBUG_LOG_W_TAG("Trying to revive CH32");
			_initExpander();
			_i2cErrorFlag = false;
			continue;
		}

		for (size_t i = 0; i < state.size(); i++)
		{
			if(abs((int)prevState[i] - (int)state[i]) > error){
				timeStates[i].timestampUs = esp_timer_get_time();
				timeStates[i].sended = false;

			}else{
				//debounce logic wait some time for stability of the signal, due to all possible jack connections
				if(((esp_timer_get_time() - timeStates[i].timestampUs) > bounceTimeUs)
					&& !timeStates[i].sended)
				{
					FB_DEBUG_LOG_I_TAG("%d = %u -> %u", i, prevState[i], state[i]);
					//here we are sure we need to send stable state
					changedFlag = true;
					timeStates[i].sended = true;
				}
			}
		}

		prevState = std::move(state);

		if(changedFlag){
			char buffer[256] = "[";
			char* ptr = &buffer[1];
			for(auto v : prevState){
				ptr = ptr + sprintf(ptr, "%d,", v);
			}
			ptr--;
			ptr[0] = ']';
			ptr[1] = '\0';

			FB_DEBUG_LOG_I_TAG("%s", buffer);
			if(settings::getUseHttp()){
				changedFlag = !_http.post(settings::getUrl(), buffer);
			}else{
				changedFlag = !_mqtt.publish(_MQTT_PATH_STATE, buffer);
			}
		}else{
			vTaskDelay(pdMS_TO_TICKS(100));
		}
	}

	vTaskDelete(NULL);
}



static void _registerMatrix()
{
	#if _HW_VERSION == 2
		_matrix.addEntry({2, 15, 14, 6, true});
		if(settings::getChipsCount() > 1){
			_matrix.addEntry({7, 6, 5, 0, true});
		}
		if(settings::getChipsCount() > 2){
			_matrix.addEntry({10, 0, 8, 4, true});
		}

	#else
		_matrix.addEntry({{6}, 0, true});
		_matrix.addEntry({{6}, 1, true});
		_matrix.addEntry({{7}, 0, true});
		_matrix.addEntry({{7}, 1, true});
		_matrix.addEntry({{8}, 0, true});
		_matrix.addEntry({{8}, 1, true});
		_matrix.addEntry({{9}, 0, true});
		_matrix.addEntry({{9}, 1, true});
		_matrix.addEntry({{0}, 0, true});
		_matrix.addEntry({{0}, 1, true});
		_matrix.addEntry({{10}, 0, true});
		_matrix.addEntry({{10}, 1, true});
	#endif
}



static void _init_from_settings()
{

}



void project::initHwObjs()
{
	_sensorService.addSensor(&_keyboardSensor);

	_init_from_settings();

	// Подписка на глобальные события (WiFi и т.д.)
	global::getEventManager()->attachListener(&_keyboardHandler);
	global::getEventManager()->attachListener(&_mqtt);

	_i2c.init(0, pins::PIN_SDA, pins::PIN_SCL);
	_i2c.addDevice(100000, EX_PROTOCOL_DEFAULT_I2C_ADDRESS >> 1);

	ex_i2c_t masterCfg;
	masterCfg.read_cb = [](int32_t address, uint8_t* data, int32_t length){
		const bool result = _i2c.read(0, {data, (unsigned int) length}, 1000);
		if(!result){
			_i2cErrorFlag = true;
		}
		return result;
	};

	masterCfg.write_cb = [](int32_t address, uint8_t* data, int32_t length){
		bool result = _i2c.write(0, {data, (unsigned int) length}, 1000);
		if(!result){
			_i2cErrorFlag = true;
		}
		return result;
	};

	ex_master_init(&_expander, &masterCfg);

	_initExpander();
	_registerMatrix();

	// Инициализация MQTT (используются те же настройки IP/порта, что и в примере с матрицей)
	_mqtt.init(settings::getIp(), settings::getPort(), 3 * 1024);
	// При необходимости можно добавить обработчик входящих сообщений (не требуется для отправки)

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