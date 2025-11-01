#include "fb_gpio_switch.hpp"

#include "driver/gpio.h"
#include "esp_log.h"



using namespace fb;
using namespace switches;



void GpioSwitch::turnOn()
{
	_init();

	esp_err_t ret = gpio_set_level(static_cast<gpio_num_t>(_gpio), 1);
	if (ret == ESP_OK) {
		_state = true;
		FB_DEBUG_LOG_I_OBJ("GPIO %d turned ON", _gpio);
	} else {
		FB_DEBUG_LOG_I_OBJ("Failed to turn ON GPIO %d: %s", _gpio, esp_err_to_name(ret));
	}
}

void GpioSwitch::turnOff()
{
	_init();

	esp_err_t ret = gpio_set_level(static_cast<gpio_num_t>(_gpio), 0);
	if (ret == ESP_OK) {
		_state = false;
		FB_DEBUG_LOG_I_OBJ("GPIO %d turned OFF", _gpio);
	} else {
		FB_DEBUG_LOG_I_OBJ("Failed to turn OFF GPIO %d: %s", _gpio, esp_err_to_name(ret));
	}
}

void GpioSwitch::_action(SwitchIface* me, bool state)
{
	GpioSwitch* tmp = static_cast<GpioSwitch*>(me);
	if(state){
		tmp->turnOn();
	}else{
		tmp->turnOff();
	}
}

void GpioSwitch::_init()
{
	if(_initFlag){
		return;
	}

	_initFlag = true;

	FB_DEBUG_LOG_I_OBJ("Initializing GPIO %d as OUTPUT", _gpio);
	
	// GPIO configuration structure
	gpio_config_t io_conf = {
		.pin_bit_mask = (1ULL << _gpio),
		.mode = GPIO_MODE_OUTPUT,
		.pull_up_en = GPIO_PULLUP_DISABLE,
		.pull_down_en = GPIO_PULLDOWN_DISABLE,
		.intr_type = GPIO_INTR_DISABLE,
	};
	
	
	// Configure GPIO with the given settings
	esp_err_t ret = gpio_config(&io_conf);
	if (ret != ESP_OK) {
		FB_DEBUG_LOG_E_OBJ("Failed to configure GPIO %d: %s", _gpio, esp_err_to_name(ret));
		return;
	}
	
	// Initialize to OFF state
	turnOff();
}
