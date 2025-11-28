#include "fb_adc.hpp"



using namespace fb;
using namespace adc;



AdcPin::~AdcPin()
{
	if(_handler){
		adc_oneshot_del_unit(_handler);
		_handler = nullptr;
	}
}

void AdcPin::init(adc_unit_t unit, adc_channel_t channel, adc_atten_t atten)
{
	adc_oneshot_unit_init_cfg_t init_config = {
		.unit_id = ADC_UNIT_1,
		.clk_src = ADC_DIGI_CLK_SRC_DEFAULT,
		.ulp_mode = ADC_ULP_MODE_DISABLE,
	};
	adc_oneshot_new_unit(&init_config, &_handler);

	adc_oneshot_chan_cfg_t config = {
		.atten = ADC_ATTEN_DB_12,              // 11dB attenuation for wider voltage range
		.bitwidth = ADC_BITWIDTH_DEFAULT,      // 12 bits by default
	};
	adc_oneshot_config_channel(_handler, ADC_CHANNEL_1, &config);
}

int AdcPin::readRaw(int samples)
{
	int sum = 0;
	int tmp = 0;
	for (int i = 0; i < samples; i++) {
		adc_oneshot_read(_handler, ADC_CHANNEL_1, &tmp);
		sum += tmp;
	}
	return sum / samples;
}
