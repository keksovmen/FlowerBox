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



		class ExpanderMatrixByMultiplexer : public debug::Named
		{
			public:
				struct Entry
				{
					int gpioPin_0;
					int gpioPin_1;
					int gpioPin_2;
					int adcPin;
					bool activeHigh;	
				};



				static const int NO_PIN = -1;



				//must init ex_master_t and pin states too
				ExpanderMatrixByMultiplexer(ex_master_t& expander, int averageCount);

				virtual const char* getName() const override;

				void addEntry(const Entry& entry);
				std::vector<uint16_t> readPins();

			private:
				const int _averageCount;
				ex_master_t& _expander;
				std::vector<Entry> _entries;



				uint16_t _readAverage(int pin);
		};
	}
}