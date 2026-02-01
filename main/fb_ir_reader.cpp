#include "fb_ir_reader.hpp"

#include "fb_ir_commands.hpp"

#include "esp_timer.h"
#include "driver/gpio.h"
#include "soc/gpio_struct.h"



using namespace fb;
using namespace periph;



/**
 * @brief 
 * 
 * @param start at begining
 * @param end at null symbol
 */
static void _reverseString(char* start, char* end)
{
	end--;

	while(end > start){
		char tmp = *start;
		*start = *end;
		*end = tmp;

		start++;
		end--;
	}
}



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
		.intr_type = GPIO_INTR_POSEDGE,
	};
	gpio_config(&cfg);
	gpio_intr_enable(static_cast<gpio_num_t>(pin));

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
	
		e.push(currentStamp);

		if(e.currentLength() > 14){
			e.setMessageFlag();
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
		ulTaskNotifyTake(pdTRUE, portMAX_DELAY);

		//must wait for some time to be sure it is final length
		//time is only thing we can use to determine end of the command
		vTaskDelay(pdMS_TO_TICKS(10));

		for(int pinIndex = 0 ; pinIndex < me->_length; pinIndex++){
			auto& e = me->_pins[pinIndex];

			if(!e.hasMessage()){
				continue;
			}

			e.clearMessageFlag();

			char buff[256];
			char* ptr = buff;
			uint32_t data = 0;
			size_t length = 0;

			for(size_t i = 0; i < e.currentLength() - 1; i++){
				const int delta = e[i + 1] - e[i];
				if(delta > 1500 && delta < 2000){
					data += 1 << i;
					ptr += sprintf(ptr, "%d ", 1);
					length++;

				}else if(delta > 900 && delta < 1400){
					ptr += sprintf(ptr, "%d ", 0);
					length++;
				}
			}

			_reverseString(buff, ptr);
			data = Ir::reverseInt(data, length);
			FB_DEBUG_LOG_I_TAG("Received on GPIO_%d: \r\n%s", e.pin, buff);

			if(length == FB_IR_COMMANDS_GLOBAL_LENGTH_BITS && e.currentLength() == (FB_IR_COMMANDS_GLOBAL_LENGTH_BITS + 1)){
				// case for HEAL and KILL commands
				std::invoke(me->_dataCb, e.pin, pinIndex, data, FB_IR_COMMANDS_GLOBAL_LENGTH_BITS);

			}else if(length == FB_IR_COMMANDS_ATTACK_LENGTH_BITS && e.currentLength() == (FB_IR_COMMANDS_ATTACK_LENGTH_BITS + 1)){
				//case for single shoot command
				std::invoke(me->_dataCb, e.pin, pinIndex, data, FB_IR_COMMANDS_ATTACK_LENGTH_BITS);
			}
		}
    }

	vTaskDelete(NULL); 
}