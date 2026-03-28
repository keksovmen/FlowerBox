#pragma once


#include <utility>

#include "driver/ledc.h"
#include "driver/gpio.h"



namespace fb
{
	namespace wrappers
	{
		class WrapperIface
		{
			public:
				virtual ~WrapperIface() = default;

				virtual void init() = 0;

				virtual void setValue(bool value) = 0;
				
				/**
				 * @brief Set the Value object
				 * 
				 * @param value [0; 100]
				 */
				virtual void setValue(int value) = 0;
		};



		class WrapperGpio : public WrapperIface
		{
			public:
				WrapperGpio(gpio_num_t gpio, bool initialState);

				virtual void init() override;

				virtual void setValue(bool value) override;
				virtual void setValue(int value) override;
			
			private:
				const gpio_num_t _gpio;
				const bool _initialState;
		};



		class WrapperPwm : public WrapperIface
		{
			public:
				WrapperPwm(ledc_timer_t timer, ledc_channel_t channel, gpio_num_t gpio, bool highSpeed);
				WrapperPwm(ledc_timer_t timer, ledc_channel_t channel, gpio_num_t gpio, int frequency);

				virtual void init() override;

				virtual void setValue(bool value) override;
				virtual void setValue(int value) override;

				void setFrequency(uint32_t frequency);
			
			private:
				static bool _timer_states[static_cast<int>(LEDC_TIMER_MAX)];



				ledc_timer_t _timer;
				ledc_channel_t _channel;
				gpio_num_t _gpio;
				int _frequency;



				int _valueToDuty(int val);
		};
	}
}