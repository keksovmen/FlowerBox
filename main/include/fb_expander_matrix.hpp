#pragma once



#include <vector>

#include "fb_debug.hpp"
#include "ex_master.h"



namespace fb
{
	namespace periph
	{
		class ExpanderMatrix : public debug::Named
		{
			public:
				struct Entry
				{
					int gpioPin;	
					int adcPin;
					bool activeHigh;	
				};



				static const int NO_PIN = -1;



				//must init ex_master_t and pin states too
				ExpanderMatrix(ex_master_t& expander);

				virtual const char* getName() const override;

				void addEntry(const Entry& entry);
				std::vector<uint16_t> readPins();

			private:
				ex_master_t& _expander;
				std::vector<Entry> _entries;
		};
	}
}