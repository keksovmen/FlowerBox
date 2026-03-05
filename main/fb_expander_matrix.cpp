#include "fb_expander_matrix.hpp"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"



using namespace fb;
using namespace periph;



ExpanderMatrix::ExpanderMatrix(ex_master_t& expander)
	: _expander(expander)
{

}

const char* ExpanderMatrix::getName() const
{
	return "ExpanderMatrix";
}

void ExpanderMatrix::addEntry(const Entry& entry)
{
	_entries.push_back(entry);
}

std::vector<uint16_t> ExpanderMatrix::readPins()
{
	std::vector<uint16_t> result;
	result.reserve(_entries.size());

	for(auto& e : _entries)
	{
		if(e.gpioPin != NO_PIN){
			ex_master_set_pin_val(&_expander, static_cast<ex_master_pin_t>(e.gpioPin), e.activeHigh);
		}

		uint16_t out = 0;
		ex_master_adc_read(&_expander, static_cast<ex_master_adc_pin_t>(e.adcPin), &out);
		result.emplace_back(out);

		if(e.gpioPin != NO_PIN){
			ex_master_set_pin_val(&_expander, static_cast<ex_master_pin_t>(e.gpioPin), !e.activeHigh);
		}
	}

	return result;
}



ExpanderMatrixByMultiplexer::ExpanderMatrixByMultiplexer(ex_master_t& expander, int averageCount)
	: _averageCount(averageCount), _expander(expander)
{

}

const char* ExpanderMatrixByMultiplexer::getName() const
{
	return "ExpanderMatrixByMultiplexer";
}

void ExpanderMatrixByMultiplexer::addEntry(const Entry& entry)
{
	_entries.push_back(entry);
}

std::vector<uint16_t> ExpanderMatrixByMultiplexer::readPins()
{
	std::vector<uint16_t> result;
	result.reserve(_entries.size() * 8);

	for(auto& e : _entries)
	{
		uint16_t out = 0;
		//0 = 000
		ex_master_set_pin_val(&_expander, static_cast<ex_master_pin_t>(e.gpioPin_0), !e.activeHigh);
		ex_master_set_pin_val(&_expander, static_cast<ex_master_pin_t>(e.gpioPin_1), !e.activeHigh);
		ex_master_set_pin_val(&_expander, static_cast<ex_master_pin_t>(e.gpioPin_2), !e.activeHigh);
		vTaskDelay(pdMS_TO_TICKS(10));
		out = _readAverage(e.adcPin);
		result.emplace_back(out);
		out = 0;
		//1 = 001
		ex_master_set_pin_val(&_expander, static_cast<ex_master_pin_t>(e.gpioPin_0), e.activeHigh);
		vTaskDelay(pdMS_TO_TICKS(10));
		out = _readAverage(e.adcPin);
		result.emplace_back(out);
		out = 0;
		//2 == 010
		ex_master_set_pin_val(&_expander, static_cast<ex_master_pin_t>(e.gpioPin_0), !e.activeHigh);
		ex_master_set_pin_val(&_expander, static_cast<ex_master_pin_t>(e.gpioPin_1), e.activeHigh);
		vTaskDelay(pdMS_TO_TICKS(10));
		out = _readAverage(e.adcPin);
		result.emplace_back(out);
		out = 0;
		//3 == 011
		ex_master_set_pin_val(&_expander, static_cast<ex_master_pin_t>(e.gpioPin_0), e.activeHigh);
		vTaskDelay(pdMS_TO_TICKS(10));
		out = _readAverage(e.adcPin);
		result.emplace_back(out);
		out = 0;
		//4 == 100
		ex_master_set_pin_val(&_expander, static_cast<ex_master_pin_t>(e.gpioPin_0), !e.activeHigh);
		ex_master_set_pin_val(&_expander, static_cast<ex_master_pin_t>(e.gpioPin_1), !e.activeHigh);
		ex_master_set_pin_val(&_expander, static_cast<ex_master_pin_t>(e.gpioPin_2), e.activeHigh);
		vTaskDelay(pdMS_TO_TICKS(10));
		out = _readAverage(e.adcPin);
		result.emplace_back(out);
		out = 0;
		//5 == 101
		ex_master_set_pin_val(&_expander, static_cast<ex_master_pin_t>(e.gpioPin_0), e.activeHigh);
		vTaskDelay(pdMS_TO_TICKS(10));
		out = _readAverage(e.adcPin);
		result.emplace_back(out);
		out = 0;
		//6 == 110
		ex_master_set_pin_val(&_expander, static_cast<ex_master_pin_t>(e.gpioPin_0), !e.activeHigh);
		ex_master_set_pin_val(&_expander, static_cast<ex_master_pin_t>(e.gpioPin_1), e.activeHigh);
		vTaskDelay(pdMS_TO_TICKS(10));
		out = _readAverage(e.adcPin);
		result.emplace_back(out);
		out = 0;
		//7 == 111
		ex_master_set_pin_val(&_expander, static_cast<ex_master_pin_t>(e.gpioPin_0), e.activeHigh);
		vTaskDelay(pdMS_TO_TICKS(10));
		out = _readAverage(e.adcPin);
		result.emplace_back(out);
		//turn all off
		ex_master_set_pin_val(&_expander, static_cast<ex_master_pin_t>(e.gpioPin_0), !e.activeHigh);
		ex_master_set_pin_val(&_expander, static_cast<ex_master_pin_t>(e.gpioPin_1), !e.activeHigh);
		ex_master_set_pin_val(&_expander, static_cast<ex_master_pin_t>(e.gpioPin_2), !e.activeHigh);
		
		// for(int gpio : e.gpioPins){
		// 	if(gpio != NO_PIN){
		// 		ex_master_set_pin_val(&_expander, static_cast<ex_master_pin_t>(gpio), e.activeHigh);
		// 	}
		// }


		// for(int gpio : e.gpioPins){
		// 	if(gpio != NO_PIN){
		// 		ex_master_set_pin_val(&_expander, static_cast<ex_master_pin_t>(gpio), !e.activeHigh);
		// 	}
		// }
	}

	return result;
}

uint16_t ExpanderMatrixByMultiplexer::_readAverage(int pin)
{
	uint32_t result = 0;
	for(int i = 0; i < _averageCount; i++){
		uint16_t out = 0;
		ex_master_adc_read(&_expander, static_cast<ex_master_adc_pin_t>(pin), &out);
		result += out;
	}

	return result / _averageCount;
}