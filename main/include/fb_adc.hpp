#pragma once



#include "esp_adc/adc_oneshot.h"



namespace fb
{
	namespace adc
	{
		class AdcPin
		{
			public:

				~AdcPin();

				void init(adc_unit_t unit, adc_channel_t channel, adc_atten_t atten);
				int readRaw(int samples);


				
			private:
				adc_oneshot_unit_handle_t _handler = nullptr;
		};
	}
}