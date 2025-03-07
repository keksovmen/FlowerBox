#include "h_keyboard.hpp"



#include "esp_log.h"



#define _TASK_NAME "Keyboard"



using namespace h;



Keyboard* Keyboard::_instance = nullptr;



Keyboard* Keyboard::instance()
{
	if(Keyboard::_instance == nullptr){
		Keyboard::_instance = new Keyboard();
	}

	return Keyboard::_instance;
}

void Keyboard::keyboardAddButton(gpio_num_t pin, ButtonVK vk)
{
	_buttons.push_back(KeyboardButton(pin, vk));

	gpio_reset_pin(pin);
	gpio_set_direction(pin, GPIO_MODE_INPUT);
	gpio_pullup_en(pin);

	if(!gpio_get_level(pin)){
		_buttons.back().press();
	}else{
		_buttons.back().release();
	}

}

bool Keyboard::keyboardStart(int stackSize, int priority, int core)
{
	if(_isTaskRunning) return true;
	_isTaskRunning = true;

	if(core == -1){
		return pdPASS == xTaskCreate(Keyboard::_task, _TASK_NAME, stackSize, this, priority, NULL);
	}else{
		// return pdPASS == xTaskCreatePinnedToCore(Keyboard::_task, _TASK_NAME, stackSize, this, priority, NULL, core);
		return pdPASS == xTaskCreate(Keyboard::_task, _TASK_NAME, stackSize, this, priority, NULL);
	}

	_isTaskRunning = false;

	return false;
}

void Keyboard::keyboardStop()
{
	_isTaskRunning = false;
}

void Keyboard::_task(void* arg)
{
	Keyboard* self = static_cast<Keyboard*>(arg);
	assert(self);

	while(self->_isTaskRunning){
		for(KeyboardButton& b : self->_buttons){
			const bool isPressed = !gpio_get_level(b.pin);

			if(isPressed){
				if(b.press()){
					//прошлое состояние pressed
					//проверяем прошл ли период генерации события
					if(b.currentDurationMs() >= self->_repeatPeriodMs){
						self->_consumer({mapButtonVKtoButtonKey(b.vk), b.holdDurationMs(), ButtonMovement::STILL_PRESSED});
						b.resetCurrentDuration();
					}
				}else{
					//прошлое состояние released
					self->_consumer({mapButtonVKtoButtonKey(b.vk), b.holdDurationMs(), ButtonMovement::PRESSED});
				}
			}else{
				if(b.release()){
					//прошлое состояние pressed
					self->_consumer({mapButtonVKtoButtonKey(b.vk), b.holdDurationMs(), ButtonMovement::RELEASED});
				}else{
					//прошлое состояние released
					//do nothing
				}
			}
		}

		vTaskDelay(pdMS_TO_TICKS(self->_pullPeriodMs));
	}

	vTaskDelete(NULL);
}



bool KeyboardButton::press()
{
	if(_isPressed) return true;

	_isPressed = true;
	_whenPressed = xTaskGetTickCount();
	_currentStart = _whenPressed;

	return false;
}

bool KeyboardButton::release()
{
	if(!_isPressed) return false;

	_isPressed = false;

	return true;
}

uint32_t KeyboardButton::holdDurationMs()
{
	return pdTICKS_TO_MS(xTaskGetTickCount() - _whenPressed);
}

uint32_t KeyboardButton::currentDurationMs()
{
	return pdTICKS_TO_MS(xTaskGetTickCount() - _currentStart);
}

void KeyboardButton::resetCurrentDuration()
{
	_currentStart = xTaskGetTickCount();
}
