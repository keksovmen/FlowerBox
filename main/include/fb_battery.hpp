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
				float readVolts();

				void setMaxRaw(int raw);
				void setMinRaw(int raw);
				void setMaxVoltage(float voltage);
				void setMinVoltage(float voltage);

			private:
				AdcPin& _adc;
				int _maxRaw = 0;
				int _minRaw = 0;
				float _maxVoltage = 0.0f;
				float _minVoltage = 0.0f;
		};
	}
}