#include "fb_washer_hw_obj.hpp"

#include "fb_globals.hpp"
#include "fb_keyboard_handler.hpp"
#include "fb_washer_pins.hpp"
#include "fb_washer_settings.hpp"
#include "fb_sensor_keyboard.hpp"
#include "fb_wrappers.hpp"



#define _BUZZER_FREQ_HZ 2800



using namespace fb;
using namespace project;



static const char* TAG = "hw";



static sensor::SensorService _sensorService;
static switches::SwitchService _swithService;

static sensor::SensorStorage _sensorStorage;

static keyboard::KeyboardHandler _keyboardHandler;

static sensor::KeyboardSensor<1> _keyboardSensor({std::pair{pins::PIN_KEYBOARD_RESET, h::ButtonVK::VK_0}});
static wrappers::WrapperPwm _buzzer(LEDC_TIMER_0, LEDC_CHANNEL_0, static_cast<gpio_num_t>(pins::BUZZER), _BUZZER_FREQ_HZ);

static h::Keyboard _matrixKeyboard;

static TaskHandle_t _melodyTaskHndl;

static int _value = 100;
static int _multiplier = 1;
static volatile bool _stopFlag = false;



static void _setDigit(uint8_t digit)
{
	// 7‑segment patterns for digits 0‑9.
	// Each bit corresponds to one segment (bit0 = a, bit1 = b, ..., bit6 = g).
	// The order of segments in pins::SEGMENTS must match this bit order.
	static const uint8_t digitPatterns[10] = {
		0b00111111, // 0: a,b,c,d,e,f
		0b00000110, // 1: b,c
		0b01011011, // 2: a,b,d,e,g
		0b01001111, // 3: a,b,c,d,g
		0b01100110, // 4: b,c,f,g
		0b01101101, // 5: a,c,d,f,g
		0b01111101, // 6: a,c,d,e,f,g
		0b00000111, // 7: a,b,c
		0b01111111, // 8: all segments
		0b01101111  // 9: a,b,c,d,f,g
	};

	if (digit > 9) return;

	// Turn off all segments first
	for (int seg : pins::SEGMENTS) {
		gpio_set_level(static_cast<gpio_num_t>(seg), 0);
	}

	const uint8_t pattern = digitPatterns[digit];
	// Turn on only the segments required for this digit
	for (int i = 0; i < 7; i++) {
		if (pattern & (1 << i)) {
			gpio_set_level(static_cast<gpio_num_t>(pins::SEGMENTS[i]), 1);
		}
	}
}



static void _displayTask(void* arg)
{
	for(;;)
	{
		//read inputs here, we have the whole 1ms of time for this
		_matrixKeyboard.tick();

		int numbers[] = {_value / 100, (_value / 10) % 10, _value % 10};
		//use only first 3 digits the forth is not interested to us
		for(int i = 0; i < 3; i++)
		{
			int digit = pins::DIGITS[i];
			_setDigit(static_cast<uint8_t>(numbers[i]));
			gpio_set_level(static_cast<gpio_num_t>(digit), 1);

			vTaskDelay(pdMS_TO_TICKS(3));
			gpio_set_level(static_cast<gpio_num_t>(digit), 0);
		}
	}
}

static void _melodyTask(void* arg)
{
	//gymn???
	// static const int NOTES_SIZE = 98;
	// static const int freqs[] = {174, 233, 174, 195, 220, 146, 146, 195, 174, 155, 174, 116, 116, 130, 130, 146, 155, 155, 174, 195, 220, 233, 261, 174, 293, 261, 233, 261, 174, 174, 233, 220, 195, 220, 146, 146, 195, 174, 155, 174, 116, 116, 233, 220, 195, 174, 293, 261, 233, 220, 233, 261, 174, 174, 233, 220, 195, 174, 195, 220, 146, 146, 233, 195, 220, 233, 195, 220, 233, 195, 233, 311, 311, 293, 261, 233, 261, 293, 233, 233, 261, 233, 220, 195, 220, 233, 195, 195, 233, 220, 195, 174, 116, 116, 174, 195, 220, 233};
	// static const int timings[] = {250, 500, 375, 125, 500, 250, 250, 500, 375, 125, 500, 250, 250, 500, 375, 125, 500, 375, 125, 500, 375, 125, 750, 250, 500, 375, 125, 500, 250, 250, 500, 375, 125, 500, 375, 125, 500, 375, 125, 500, 375, 125, 500, 375, 125, 1000, 1000, 250, 250, 250, 250, 750, 250, 1000, 1000, 250, 250, 250, 250, 750, 250, 1000, 500, 375, 125, 500, 375, 125, 500, 375, 125, 1000, 1000, 250, 250, 250, 250, 750, 250, 1000, 1000, 250, 250, 250, 250, 750, 250, 1000, 500, 375, 125, 500, 375, 125, 1000, 500, 500, 2000};
	// static const int delays[] = {278, 556, 417, 139, 556, 278, 278, 556, 417, 139, 556, 278, 278, 556, 417, 139, 556, 417, 139, 556, 417, 139, 833, 278, 556, 417, 139, 556, 278, 278, 556, 417, 139, 556, 417, 139, 556, 417, 139, 556, 417, 139, 556, 417, 139, 1111, 1111, 278, 278, 278, 278, 833, 278, 1111, 1111, 278, 278, 278, 278, 833, 278, 1111, 556, 417, 139, 556, 417, 139, 556, 417, 139, 1111, 1111, 278, 278, 278, 278, 833, 278, 1111, 1111, 278, 278, 278, 278, 833, 278, 1111, 556, 417, 139, 556, 417, 139, 1111, 556, 556, 2222};
	//nokia???
	// static const int NOTES_SIZE = 14;
	// static const int freqs[] = {329, 392, 392, 329, 440, 392, 440, 392, 440, 392, 440, 392, 440, 493};
	// static const int timings[]  = {300, 600, 300, 600, 300, 300, 300, 300, 300, 300, 300, 300, 300, 600 };
	// static const int delays[]  = {300, 1200, 300, 1200, 300, 300, 300, 300, 300, 300, 300, 300, 300, 600 };
	//harry potter
	static const int NOTES_SIZE = 62;
	static const int freqs[] = {0, 294, 392, 466, 440, 392, 587, 523, 440, 392, 466, 440, 349, 415, 294, 294, 392, 466, 440, 392, 587, 698, 659, 622, 494, 622, 587, 554, 277, 494, 392, 466, 587, 466, 587, 466, 622, 587, 554, 440, 466, 587, 554, 277, 294, 587, 0, 466, 587, 466, 587, 466, 698, 659, 622, 494, 622, 587, 554, 277, 466, 392};
	static const int timings[] = {750, 375, 563, 188, 375, 750, 375, 1125, 1125, 563, 188, 375, 750, 375, 2250, 375, 563, 188, 375, 750, 375, 750, 375, 750, 375, 563, 188, 375, 750, 375, 2250, 375, 750, 375, 750, 375, 750, 375, 750, 375, 563, 188, 375, 750, 375, 2250, 375, 375, 750, 375, 750, 375, 750, 375, 750, 375, 563, 188, 375, 750, 375, 2250};
	static const int delays[] = {833, 417, 625, 208, 417, 833, 417, 1250, 1250, 625, 208, 417, 833, 417, 2500, 417, 625, 208, 417, 833, 417, 833, 417, 833, 417, 625, 208, 417, 833, 417, 2500, 417, 833, 417, 833, 417, 833, 417, 833, 417, 625, 208, 417, 833, 417, 2500, 417, 417, 833, 417, 833, 417, 833, 417, 833, 417, 625, 208, 417, 833, 417, 2500 };
	_buzzer.setValue(0);

	for(;;)
	{
		ulTaskNotifyTake(pdTRUE, portMAX_DELAY);

		for(int i = 0; i < NOTES_SIZE; i++)
		{
			if(_stopFlag){
				_stopFlag = false;
				break;
			}

			if(freqs[i] != 0){
				_buzzer.setFrequency(freqs[i]);
				_buzzer.setValue(50);
			}
			vTaskDelay(pdMS_TO_TICKS(timings[i]));

			_buzzer.setValue(0);
			vTaskDelay(pdMS_TO_TICKS((delays[i] - timings[i])));
		}
	}
}

static void _ringTask(void* arg)
{
	_buzzer.setFrequency(_BUZZER_FREQ_HZ);

	for(int i = 0; i < 3; i++){
		_buzzer.setValue(50);
		vTaskDelay(pdMS_TO_TICKS(250));
		_buzzer.setValue(0);
		vTaskDelay(pdMS_TO_TICKS(250));
	}

	vTaskDelete(NULL);
}



static void _playBuzz()
{
	xTaskCreate(&_ringTask, "BUZZER", 4 * 1024, NULL, 16, NULL);
}

static void _startMelody()
{
	xTaskNotifyGive(_melodyTaskHndl);
}

static void _stopMelody()
{
	_stopFlag = true;
}



static void _keyHandler(const h::ButtonAction& action)
{
	FB_DEBUG_LOG_I_TAG("Button action: key = %d, mov = %d, %d ms", (int) action.button, (int) action.movement, (int) action.holdMs);

	if(action.isJustPressed(h::ButtonKeys::PLAY_STOP)){
		_playBuzz();

	}else if(action.isLongJustPressed(h::ButtonKeys::UP, 2000)){
		//notify melody task
		_startMelody();

	}else if(action.isLongJustPressed(h::ButtonKeys::DOWN, 2000)){
		//notify melody task
		_stopMelody();

	}else if(action.isLongJustPressed(h::ButtonKeys::UP, 50)){
		_multiplier *= 10;
		_multiplier = (_multiplier > 100) ? 1 : _multiplier;
		FB_DEBUG_LOG_I_TAG("Multiplier = %d", _multiplier);

	}else if(action.isJustPressed(h::ButtonKeys::DOWN)){
		_value += _multiplier;
		_value = (_value > 1000) ? _value - 1000 : _value;
		FB_DEBUG_LOG_I_TAG("Value = %d", _value);
	}
}



static void _init_from_settings()
{
	//init keyboard here
}



void project::initHwObjs()
{
	gpio_config_t cfg = {
		.pin_bit_mask = 0,
		.mode = GPIO_MODE_OUTPUT,
		.pull_up_en = GPIO_PULLUP_DISABLE,
		.pull_down_en = GPIO_PULLDOWN_DISABLE,
		.intr_type = GPIO_INTR_DISABLE,
	};
	for(int seg : pins::SEGMENTS){
		cfg.pin_bit_mask = 1ull << seg;

		gpio_reset_pin(static_cast<gpio_num_t>(seg));
		assert(gpio_config(&cfg) == ESP_OK);
		gpio_set_level(static_cast<gpio_num_t>(seg), 0);
	}

	for(int digit : pins::DIGITS)
	{
		cfg.pin_bit_mask = 1ull << digit;

		gpio_reset_pin(static_cast<gpio_num_t>(digit));
		assert(gpio_config(&cfg) == ESP_OK);
		gpio_set_level(static_cast<gpio_num_t>(digit), 0);
	}


	_buzzer.init();


	for(int i = 0; i < sizeof(pins::DIGITS) / sizeof(pins::DIGITS[0]); i++){
		_matrixKeyboard.keyboardAddButton(new h::KeyboardMatrixButton(
			static_cast<gpio_num_t>(pins::KEYBOARD_INPUT),
			static_cast<gpio_num_t>(pins::DIGITS[i]),
			(h::ButtonVK)((int)h::ButtonVK::VK_2)));
	}

	//add first washer machine key as up key
	//TODO: uncomment
	// _matrixKeyboard.keyboardAddButton(new h::KeyboardMatrixButton(
	// 		static_cast<gpio_num_t>(pins::KEYBOARD_INPUT),
	// 		static_cast<gpio_num_t>(pins::DIGITS[3]),
	// 		(h::ButtonVK)((int)h::ButtonVK::VK_2)));
	//add second washer machine key as down key
	_matrixKeyboard.keyboardAddButton(new h::KeyboardButton(static_cast<gpio_num_t>(pins::PIN_WASHER_2_INPUT), h::ButtonVK::VK_3));
	_matrixKeyboard.setConsumer(&_keyHandler);

	_init_from_settings();

	_sensorService.addSensor(&_keyboardSensor);

	//register key handler for dropping WIFI settings
	global::getEventManager()->attachListener(&_keyboardHandler);

	xTaskCreate(&_displayTask, "SCREEN_DRIVER", 4 * 1024, NULL, 15, NULL);
	xTaskCreate(&_melodyTask, "MELODY", 4 * 1024, NULL, 16, &_melodyTaskHndl);
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