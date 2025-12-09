#include "fb_wrappers.hpp"

#include <cmath>



using namespace fb;
using namespace wrappers;







WrapperGpio::WrapperGpio(gpio_num_t gpio, bool initialState)
	: _gpio(gpio), _initialState(initialState)
{
	
}

void WrapperGpio::init()
{
	gpio_config_t cfg = {
		.pin_bit_mask = 1ULL << _gpio,
		.mode = GPIO_MODE_OUTPUT,
		.pull_up_en = GPIO_PULLUP_DISABLE,
		.pull_down_en = GPIO_PULLDOWN_DISABLE,
		.intr_type = GPIO_INTR_DISABLE,
	};
	assert(gpio_config(&cfg) == ESP_OK);

	setValue(_initialState);
}

void WrapperGpio::setValue(bool value)
{
	gpio_set_level(_gpio, value);
}

void WrapperGpio::setValue(int value)
{
	setValue(value > 0);
}



bool WrapperPwm::_timer_states[] = {false};

WrapperPwm::WrapperPwm(ledc_timer_t timer, ledc_channel_t channel, gpio_num_t gpio, bool highSpeed)
	: _timer(timer), _channel(channel), _gpio(gpio), _isHighSpeed(highSpeed)
{
	
}

void WrapperPwm::init()
{
	if(!_timer_states[std::to_underlying(_timer)]){
		ledc_timer_config_t timerCfg = {
			.speed_mode = LEDC_LOW_SPEED_MODE,
			.duty_resolution = LEDC_TIMER_8_BIT,
			.timer_num = _timer,
			.freq_hz = _isHighSpeed ? 300000ul : 4000,
			.clk_cfg = LEDC_USE_RC_FAST_CLK,
			.deconfigure = false,
		};
		ESP_ERROR_CHECK(ledc_timer_config(&timerCfg));

		_timer_states[std::to_underlying(_timer)] = true;
	}

	ledc_channel_config_t channelCfg = {
		.gpio_num = _gpio,
		.speed_mode = LEDC_LOW_SPEED_MODE,
		.channel = _channel,
		.intr_type = LEDC_INTR_DISABLE,
		.timer_sel = _timer,
		.duty = 0,
		.hpoint = 0,
		.sleep_mode = LEDC_SLEEP_MODE_KEEP_ALIVE,
		.flags = 0,
	};
	ESP_ERROR_CHECK(ledc_channel_config(&channelCfg));

	// gpio_sleep_sel_dis(_gpio);
}

void WrapperPwm::setValue(bool value)
{
	setValue(value ? 100 : 0);
}

void WrapperPwm::setValue(int value)
{
	const int duty = _valueToDuty(value);

	ESP_ERROR_CHECK(ledc_set_duty(LEDC_LOW_SPEED_MODE, _channel, duty));
	ESP_ERROR_CHECK(ledc_update_duty(LEDC_LOW_SPEED_MODE, _channel));
}

int WrapperPwm::_valueToDuty(int val)
{
	const float maxValue = 1 << (int) LEDC_TIMER_8_BIT;
	return round((val / 100.0) * maxValue);
}