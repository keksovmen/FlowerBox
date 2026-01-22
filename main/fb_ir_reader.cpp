#include "fb_ir_reader.hpp"

#include "esp_timer.h"
#include "driver/gpio.h"
#include "soc/gpio_struct.h"



using namespace fb;
using namespace periph;



const char* IrReader::getName() const
{
	return "IrReader";
}

void IrReader::init()
{
	ESP_ERROR_CHECK(gpio_isr_register(&_irq, this, ESP_INTR_FLAG_IRAM, NULL));
}

void IrReader::addPin(int pin)
{
	gpio_config_t cfg = {
		.pin_bit_mask = 1ULL << pin,
		.mode = GPIO_MODE_INPUT,
		.pull_up_en = GPIO_PULLUP_DISABLE,
		.pull_down_en = GPIO_PULLDOWN_DISABLE,
		.intr_type = GPIO_INTR_NEGEDGE,
	};
	gpio_config(&cfg);
	gpio_intr_enable(static_cast<gpio_num_t>(pin));

	// ::new((void*)&_pins[_length])Entry<25>{pin};
	_pins[_length++].pin = pin;
}

void IrReader::startTask(int priority, int stackSize, DataCb cb)
{
	_dataCb = cb;
	xTaskCreate(&_task, "Pooling", stackSize, this, priority, &_taskHndl);
}



void IRAM_ATTR IrReader::_irq(void* arg) 
{
	IrReader* me = static_cast<IrReader*>(arg);
	//we have 200000 clock cycles (running at 160Mhz) at our use or 1250 us of time

	// Read and clear interrupt status as quickly as possible
    auto status = GPIO.status.val;          // for ESP32; use correct register for your chip
    GPIO.status_w1tc.val = status;              // clear

	const auto currentStamp = esp_timer_get_time();
	for(int i = 0 ; i < me->_length; i++){
		auto& e = me->_pins[i];
		if(!(status & BIT(me->_pins[i].pin))){
			continue;
		}

		//check if it was a noise due to big spacing in time between calls
		if((static_cast<uint64_t>(currentStamp - e.lastStamp(currentStamp))) > 10000u){
			e.reset();
		}

		if(e.push(currentStamp)){
			// send to queue we have a message
			BaseType_t shouldYield;
			vTaskNotifyGiveFromISR(me->_taskHndl, &shouldYield);
		}
	}
}

// The function must be in IRAM to prevent suspension during flash operations
void IRAM_ATTR IrReader::_task(void *pvParameters)
{
	IrReader* me = static_cast<IrReader*>(pvParameters);
	const char* TAG = me->getName();

    while (true) {
		ulTaskNotifyTake(pdFALSE, portMAX_DELAY);

		for(int i = 0 ; i < me->_length; i++){
			auto& e = me->_pins[i];

			if(!e.hasMessage()){
				continue;
			}

			e.clearMessageFlag();

			char buff[128];
			char* ptr = buff;
			uint32_t data = 0;
			for(size_t i = 0; i < e.N - 1; i++){
				const int delta = e[i + 1] - e[i];
				int result = -1;
				if(delta > 2900){
					result = 2;
				}else if(delta > 1700){
					result = 1;
					data = (data << 1) + 1;
				}else{
					data = (data << 1);
					result = 0;
				}
				ptr += sprintf(ptr, "%d ", result);
			}
			FB_DEBUG_LOG_I_TAG("Triggered: %d, \r\n%s", e.pin, buff);
			std::invoke(me->_dataCb, e.pin, data);
		}
    }

	vTaskDelete(NULL); 
}