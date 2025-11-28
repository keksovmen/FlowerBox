#pragma once



#include "fb_adc.hpp"



namespace fb
{
	namespace adc
	{
		class Battery
		{
			public:
				Battery(AdcPin& pin);

				int readCharge();

				void setMaxRaw(int raw);
				void setMinRaw(int raw);

			private:
				AdcPin& _adc;
				int _maxRaw = 0;
				int _minRaw = 0;
		};
	}
}