#include "fb_expander_matrix.hpp"



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
