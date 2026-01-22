#include "fb_ir_transmitter.hpp"

#include <algorithm>

#include "esp_timer.h"
#include "driver/gpio.h"



using namespace fb;
using namespace periph;



portMUX_TYPE IrTransmitter::_lock = portMUX_INITIALIZER_UNLOCKED;



const char* IrTransmitter::getName() const
{
	return "IrTransmitter";
}

void IrTransmitter::addGpio(int gpio)
{
	if(_hasGpio(gpio)){
		FB_DEBUG_LOG_W_OBJ("Already has GPIO_%d as output", gpio);
		return;
	}

	//configure gpio as output and put inside the vector
	gpio_config_t cfg = {
        .pin_bit_mask = (1ULL << gpio),
        .mode = GPIO_MODE_OUTPUT,
        .pull_up_en = GPIO_PULLUP_DISABLE,
        .pull_down_en = GPIO_PULLDOWN_DISABLE,
        .intr_type = GPIO_INTR_DISABLE,
    };
    gpio_config(&cfg);

	_gpios.push_back(gpio);
}

void IrTransmitter::sendHeal(int gpio)
{
	if(!_hasGpio(gpio)){
		FB_DEBUG_LOG_E_OBJ("Does not have GPIO_%d as output!", gpio);
		return;
	}

	FB_DEBUG_LOG_I_OBJ("Sending HEAL on GPIO_%d", gpio);
	//we are little endian, don't need to do anything
	uint32_t data = 0x17A0C1;
	const uint8_t* cmd = ((uint8_t*) &data);
	_sendBytes(gpio, cmd, 3);

}

void IrTransmitter::sendKill(int gpio)
{
	if(!_hasGpio(gpio)){
		FB_DEBUG_LOG_E_OBJ("Does not have GPIO_%d as output!", gpio);
		return;
	}

	FB_DEBUG_LOG_I_OBJ("Sending KILL on GPIO_%d", gpio);

	uint32_t data = 0x1700C1;
	const uint8_t* cmd = ((uint8_t*) &data);
	_sendBytes(gpio, cmd, 3);
}

void IRAM_ATTR IrTransmitter::_delayUs(int64_t us)
{
	const int64_t startUs = esp_timer_get_time();
	int64_t endUs = esp_timer_get_time();

	while(endUs - startUs < us){
		endUs = esp_timer_get_time();
	}
}

void IRAM_ATTR IrTransmitter::_sendCarrier(int gpio, int times)
{
	bool state = true;
	const uint8_t delays[] = {6, 12};
	for(int i = 0; i < times; i++)
	{
		gpio_set_level(static_cast<gpio_num_t>(gpio), state);
		_delayUs(delays[state ? 0 : 1] - 1);
		state = !state;
	}

	gpio_set_level(static_cast<gpio_num_t>(gpio), 1);
	_delayUs(6);
	gpio_set_level(static_cast<gpio_num_t>(gpio), 0);
	_delayUs(639);
}


void IRAM_ATTR IrTransmitter::_sendStart(int gpio)
{
	// bool state = true;
	// const uint8_t delays[] = {6, 12};
	// for(int i = 0; i < 131 * 2; i++)
	// {
	// 	gpio_set_level(static_cast<gpio_num_t>(gpio), state);
	// 	_delayUs(delays[state ? 0 : 1] - 1);
	// 	state = !state;
	// }

	// gpio_set_level(static_cast<gpio_num_t>(gpio), 1);
	// _delayUs(6);
	// gpio_set_level(static_cast<gpio_num_t>(gpio), 0);
	// _delayUs(639);
	_sendCarrier(gpio, 262);
}

void IRAM_ATTR IrTransmitter::_sendZero(int gpio)
{
	// bool state = true;
	// const uint8_t delays[] = {6, 12};
	// for(int i = 0; i < 64; i++)
	// {
	// 	gpio_set_level(static_cast<gpio_num_t>(gpio), state);
	// 	_delayUs(delays[state ? 0 : 1] - 1);
	// 	state = !state;
	// }

	// gpio_set_level(static_cast<gpio_num_t>(gpio), 1);
	// _delayUs(6);
	// gpio_set_level(static_cast<gpio_num_t>(gpio), 0);
	// _delayUs(648);
	_sendCarrier(gpio, 64);

}

void IRAM_ATTR IrTransmitter::_sendOne(int gpio)
{
	// bool state = true;
	// const uint8_t delays[] = {6, 12};
	// for(int i = 0; i < 131; i++)
	// {
	// 	gpio_set_level(static_cast<gpio_num_t>(gpio), state);
	// 	_delayUs(delays[state ? 0 : 1] - 1);
	// 	state = !state;
	// }

	// gpio_set_level(static_cast<gpio_num_t>(gpio), 1);
	// _delayUs(6);
	// gpio_set_level(static_cast<gpio_num_t>(gpio), 0);
	// _delayUs(648);
	_sendCarrier(gpio, 131);
}

void IRAM_ATTR IrTransmitter::_sendByte(int gpio, uint8_t data)
{
	for(int i = 0; i < 8; i++){
		if((data >> i) & 0x01){
			_sendOne(gpio);
		}else{
			_sendZero(gpio);
		}
	}
}

void IRAM_ATTR IrTransmitter::_sendBytes(int gpio, const uint8_t* data, int length)
{
	portENTER_CRITICAL(&_lock);

	_sendStart(gpio);
	for(int i = 0; i < length; i++){
		_sendByte(gpio, data[i]);
	}

	portEXIT_CRITICAL(&_lock);
}

bool IrTransmitter::_hasGpio(int gpio)
{
	return std::find(_gpios.begin(), _gpios.end(), gpio) != _gpios.end();
}